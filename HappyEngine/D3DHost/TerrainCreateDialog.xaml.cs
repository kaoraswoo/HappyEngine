using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Threading;

namespace D3DHost
{
    /// <summary>
    /// Interaction logic for TerrainCreateDialog.xaml
    /// </summary>
    public partial class TerrainCreateDialog : Window
    {
        float m_scale = 4.0f;
        float m_heightScale = 0.2f;
        int m_tile = 32;
        int m_cell = 8;

        public float i_scale
        {
            get
            {

                m_scale = (float)System.Convert.ToSingle(this.VertexTerm.Text);
                return m_scale;
            }
            set
            {
                m_scale = value;
                this.VertexTerm.Text = value.ToString();
            }

        }

        public float i_heightscale
        {
            get
            {
                m_heightScale = (float)System.Convert.ToSingle(this.HeightScale.Text);
                return m_heightScale;
            }
            set
            {
                m_heightScale = value;
                this.HeightScale.Text = value.ToString();
            }
        }

        public int i_tilenum
        {
            get
            {
                m_tile = Int32.Parse(this.TileNum.Text);
                return m_tile;
            }
            set
            {
                m_tile = value;
                this.TileNum.Text = value.ToString();
            }
        }

        public int i_vertexnum
        {
            get
            {
                m_cell = Int32.Parse(this.VertexNum.Text);
                return m_cell;
            }
            set
            {
                m_cell = value;
                this.VertexNum.Text = value.ToString();
            }
        }


        public TerrainCreateDialog()
        {
            InitializeComponent();
            /*
             i_scale = scale;
             i_heightscale = heghtscale;
             i_tilenum = tilenum;
             i_vertexnum = vertexnum;
             */
           
        }
        
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            DialogResult  = true;
            this.Close();
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            DialogResult  = false;
            this.Close();
        }
    }
}
