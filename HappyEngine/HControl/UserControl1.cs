using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using ManagedHEngine;

namespace HControl
{
    public partial class HEngineControl : UserControl
    {
        static int DXWidth = 1920;
        static int DXHeight = 1080;
        int m_PictureWidth;
        int m_PictureHeight;

        //TerrainEditMode (Index : 0 알파맵수정 / Index : 1 Height수정)
        public int m_TerrainEditMode;

        
        // 알파텍스쳐 지울지, 그릴지 Draw mode선택
        //알파맵 텍스쳐 draw mode (false : draw / true : erase )
        bool m_bErase = false;
        public bool AlphaErase
        {
            set { m_bErase = value; }
            get { return m_bErase; }
        }
        //선택된 알파맵 텍스쳐 인덱스 (0~3)
        int m_selectTexture = 0;
        public int AlphaSelectedIndex
        {
            set { m_selectTexture = value; }
            get { return m_selectTexture; }
        }

        string m_nodeName="";


        public ManagedHEngine.ManagedHappyScene m_HEngine;
        bool mouseLeftDownEvent = false;
        bool mouseRightDownEvent = false;
        int m_oldMouseX, m_oldMouseY;

        [DllImport("user32.dll")]
        static extern short GetAsyncKeyState(Keys key);

        public static bool IsKeyDown(Keys key)
        {
            return (GetAsyncKeyState(key) & 0x8000) != 0;
        }

        public HEngineControl()
        {
            //Application.Idle += new System.EventHandler(this.Form_Idle);
            InitializeComponent();
            //현재 picturebox크기
            m_PictureWidth = pictureBox1.Width;
            m_PictureHeight = pictureBox1.Height;

            m_HEngine = new ManagedHEngine.ManagedHappyScene();

            m_HEngine.Create(pictureBox1.Handle);
            //m_HEngine.Render(m_nodeName);
            //m_HEngine.TerrainRender();

            //Picturebox 를 컨트롤 크기에 꽉차게한다
            pictureBox1.Bounds = this.ClientRectangle;
            pictureBox1.Dock = DockStyle.None;

            
            
        }
        /*
        //윈폼과 달리 컨트롤에서는 안먹힌다.
        public void Form_Idle(object sender, System.EventArgs e)
        {
            m_HEngine.Render();
            Invalidate();
        }
        */

        protected override void OnPaint(PaintEventArgs e)
        {
            if (m_nodeName=="TileTerrain")
            {
                m_HEngine.TerrainRender();

                if (mouseLeftDownEvent == true)
                {
                    if (m_TerrainEditMode ==0)
                    {
                        //Splatting Draw/Delete
                        m_HEngine.DrawAlpha(AlphaSelectedIndex, AlphaErase);
                    }
                    else if (m_TerrainEditMode ==1)
                    {
                        //HeightMap Up/Down
                        m_HEngine.UpdateHeight(0.05f);
                    }
                }
            }
            else
            {
                m_HEngine.Render(m_nodeName);
            }
        }

        public void Render(string nodeName)
        {
            m_nodeName = nodeName;
            //m_HEngine.Render(m_nodeName);
            Invalidate();
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            //포커스를 맞추어야 Preview Key이벤트도 발생한다
            //PictureBox p = sender as PictureBox;
            UserControl p = this;
            p.Focus();


            if (e.Button == MouseButtons.Right)
            {
                mouseRightDownEvent = true;
                m_oldMouseX = (int)e.X * DXWidth / m_PictureWidth;
                m_oldMouseY = (int)e.Y * DXHeight / m_PictureHeight;
            }

            if (e.Button == MouseButtons.Left)
            {
                mouseLeftDownEvent = true;
               
            }
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (mouseRightDownEvent == true)
            {
                // Camera Moving
                int xCooldinate =  (int)e.X * DXWidth / m_PictureWidth;
                int yCooldinate = (int)e.Y * DXHeight / m_PictureHeight;
                label1.Text = xCooldinate.ToString() + "," +yCooldinate.ToString();
                Point pos = new Point();
                pos.X = xCooldinate - m_oldMouseX;
                pos.Y = yCooldinate - m_oldMouseY;
                m_HEngine.SetCamera(pos.X, pos.Y);

                m_oldMouseX = xCooldinate;
                m_oldMouseY = yCooldinate;

            }
            if (mouseLeftDownEvent == true)
            {
                
            }
            if (m_nodeName=="TileTerrain")
            {
                int arrageX = (int)e.X * DXWidth / m_PictureWidth;
                int arrageY = (int)e.Y * DXHeight / m_PictureHeight;
                // Picking Ray to Tile Terrain
                m_HEngine.PickRay(arrageX, arrageY);
            }
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            mouseLeftDownEvent = false;
            mouseRightDownEvent = false;
        }

        private void HEngineControl_Resize(object sender, EventArgs e)
        {
            Control control = (Control)sender;
            control.Size = new Size(control.Size.Width, control.Size.Height);
            pictureBox1.Size = new Size(control.Size.Width, control.Size.Height);
            m_PictureWidth = control.Size.Width;
            m_PictureHeight = control.Size.Height;
        }

        protected override bool ProcessCmdKey(ref System.Windows.Forms.Message msg, Keys keyData)
        {
            Keys key = keyData & ~(Keys.Shift | Keys.Control);
 
            return base.ProcessCmdKey(ref msg, keyData);
        }

        private void HEngineControl_KeyPress(object sender, KeyPressEventArgs e)
        {
            /*
            if (e.KeyChar == 'a' || e.KeyChar == 'A')
            {
                m_HEngine.MoveXAxis(-4.0f);
            }
            if (e.KeyChar == 'd' || e.KeyChar == 'D')
            {
                m_HEngine.MoveXAxis(4.0f);
            }
            if (e.KeyChar == 'w' || e.KeyChar == 'W')
            {
                m_HEngine.MoveZAxis(4.0f);
            }
            if (e.KeyChar == 's' || e.KeyChar == 'S')
            {
                m_HEngine.MoveZAxis(-4.0f);
            }
             * */
        }

        private void HEngineControl_KeyDown(object sender, KeyEventArgs e)
        {
            /*
            if (e.KeyCode == Keys.A)
            {
                m_HEngine.MoveXAxis(-4.0f);
            }
            if (e.KeyCode == Keys.D)
            {
                m_HEngine.MoveXAxis(4.0f);
            }
            if (e.KeyCode == Keys.W)
            {
                m_HEngine.MoveZAxis(4.0f);
            }
            if (e.KeyCode == Keys.S)
            {
                m_HEngine.MoveZAxis(-4.0f);
            }
             */
            /*
            byte[] keys = new byte[255];

            GetKeyboardState(keys);

            if (keys[(int)Keys.A] == 129 )
            {
                m_HEngine.MoveXAxis(-4.0f);
            }

            if (keys[(int)Keys.D] == 129)
            {
                m_HEngine.MoveXAxis(4.0f);
            }

            if (keys[(int)Keys.W] == 129)
            {
                m_HEngine.MoveZAxis(4.0f);
            }

            if (e.KeyCode == Keys.S)
            {
                m_HEngine.MoveZAxis(-4.0f);
            }
            */
            float dx = 0.0f;
            dx += IsKeyDown(Keys.A) ? -1.0f : 0.0f;
            dx += IsKeyDown(Keys.D) ? +1.0f : 0.0f;

            float dz = 0.0f;
            dz += IsKeyDown(Keys.W) ? +1.0f : 0.0f;
            dz += IsKeyDown(Keys.S) ? -1.0f : 0.0f;

            float multiplier = 1.0f;
            multiplier *= IsKeyDown(Keys.LShiftKey) ? 3.0f : 1.0f;

            m_HEngine.MoveXAxis(dx * multiplier);
            m_HEngine.MoveZAxis(dz * multiplier);
        }


    }
}
