using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using System.Runtime.InteropServices;
using System.Security.Permissions;

using ManagedHEngine;

namespace TestWinForm
{
    public partial class Form1 : Form
    {
        bool mouseLeftDownEvent = false;
        bool mouseRightDownEvent = false;
        int m_oldMouseX, m_oldMouseY;
        
        /* // WPF
        [DllImport("D3DContent.dll")]
        public static extern int TestCall();

        //[System.Security.SuppressUnmanagedCodeSecurity]
        [DllImport("D3DContent.dll", CharSet=CharSet.Auto)]
        public static extern void InitHappy(IntPtr hWnd);
        */

        ManagedHEngine.ManagedHappyScene m_HEngine;


        public Form1()
        {
            Application.Idle += new System.EventHandler(this.Form_Idle);
            InitializeComponent();
            //InitHappy(pictureBox1.Handle); // WPF

            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form_Closed);


            m_HEngine = new ManagedHEngine.ManagedHappyScene();

            m_HEngine.Create(pictureBox1.Handle);
            //m_HEngine.Render();

           

        }

        private void Form_Idle(object sender, System.EventArgs e)
        {
            //m_HEngine.Render();
            Invalidate();
        }

        private void Form_Closed(object sender, System.EventArgs e)
        {
            m_HEngine.Destroy();
        }


    

        protected override void OnPaint(PaintEventArgs e)
        {
            //m_HEngine.Render();
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            //포커스를 맞추어야 Preview Key이벤트도 발생한다
            PictureBox p = sender as PictureBox;
            p.Focus();
            

            if (e.Button == MouseButtons.Right)
            {
                mouseRightDownEvent = true;
                m_oldMouseX = (int)e.X;
                m_oldMouseY = (int)e.Y;
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
                int xCooldinate = (int)e.X;
                int yCooldinate = (int)e.Y;
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
                //Splatting Draw/Delete
                //m_HEngine.DrawAlpha();
                //HeightMap Up/Down

            }
            // Picking Ray to Tile Terrain
            m_HEngine.PickRay((int)e.X, (int)e.Y);

        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            mouseLeftDownEvent = false;
            mouseRightDownEvent = false;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_HEngine.SaveTerrain("C:\\Project\\TestAlpha\\Test.terrain");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            m_HEngine.LoadTerrain("C:\\Project\\TestAlpha\\Test.terrain");
        }

        private void pictureBox1_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
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
        }

        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
           
        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            //m_HEngine.SelectTexture(0);
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
           // m_HEngine.SelectTexture(1);
        }

        private void radioButton3_CheckedChanged(object sender, EventArgs e)
        {
           // m_HEngine.SelectTexture(2);
        }

        private void radioButton4_CheckedChanged(object sender, EventArgs e)
        {
          //  m_HEngine.SelectTexture(3);
        }

        private void radioButton5_CheckedChanged(object sender, EventArgs e)
        {
           // m_HEngine.SelectDelMode(false);
        }

        private void radioButton6_CheckedChanged(object sender, EventArgs e)
        {
           // m_HEngine.SelectDelMode(true);
        }

    }
}
