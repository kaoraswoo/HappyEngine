
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Runtime.InteropServices;
using System.Security.Permissions;

using System.Windows.Forms.Integration;

using System.Activities.Presentation.PropertyEditing;
using HControl;
using System.ComponentModel;


namespace D3DHost
{
    public partial class MainWindow : Window
    {
        bool mouseDownEvent = false;
        int m_oldMouseX, m_oldMouseY;

        HControl.HEngineControl m_HControl;
        HControl.HEngineControl m_HControl2;

        AlphaMapInfo[] m_alphaprop;

        string m_selectNode="TileTerrain";

        public MainWindow()
        {
            InitializeComponent();
            new SplashWindow().ShowDialog();
           
            //윈폼컨트롤 생성테스트
            //WindowsFormsHost host = new WindowsFormsHost();
            m_HControl = new HControl.HEngineControl();
            m_HControl2 = new HControl.HEngineControl();
            hostWinForm.Child = m_HControl;
            //hostWinForm2.Child = m_HControl2;
           
            
 
            // 프로퍼티 그리드에 적용  
            //propertyGrid1.SelectedObject = prop;
            m_alphaprop = new AlphaMapInfo[4];
            for (int i=0; i<4; i++)
            {
                m_alphaprop[i] = new AlphaMapInfo();
                string strtexture = m_HControl.m_HEngine.GetTexturePath(i);
                m_alphaprop[i].SetData(i, strtexture);
                m_alphaprop[i].PropertyChanged += new PropertyChangedEventHandler(this.TexturePathChanged);  
            }
            AlphaTextureInfo.SelectedObject = m_alphaprop[0];
            AlphaTextureInfo.ToolbarVisible = false;
            

            //Layer TreeView
            //this.scenodeList.ItemsSource = m_HControl.m_HEngine.GetSceneList();
            //this.scenodeTree.ItemsSource = m_HControl.m_HEngine.GetSceneList();
            TreeViewItem newItemTerrain = new TreeViewItem();
            newItemTerrain.Header = "TileTerrain";
            this.scenodeTree.Items.Add(newItemTerrain);
            

            TreeViewItem newItemC1 = new TreeViewItem();
            newItemC1.Header = "전체";
            newItemC1.ItemsSource = m_HControl.m_HEngine.GetSceneList();
            this.scenodeTree.Items.Add(newItemC1);
           

            //알파맵 DrawMode
            if (m_HControl.AlphaErase)
            {
                radiodraw.IsChecked = false;
                radioerase.IsChecked = true;
            }
            else
            {
                radiodraw.IsChecked = true;
                radioerase.IsChecked = false;
            }
            //알파맵 리스트박스
            AlphaMapList.SelectedIndex = m_HControl.AlphaSelectedIndex;


            //Height 모드 ( 0: Up   1: Down   2: 평탄화   3: 평지화 )
            switch (m_HControl.m_HEngine.GetHeightEditMode())
            {
                case 0:
                    this.HeightUp.IsChecked = true;
                    break;
                case 1:
                    this.HeightDown.IsChecked = true;
                    break;
                case 2:
                    this.HeightFlat.IsChecked = true;
                    break;

                case 3:
                    this.HeightFlatLand.IsChecked = true;
                    break;
            }
            
            //LOD Rate
            this.LODLEVEL.Text = m_HControl.m_HEngine.GetLODrate().ToString();
            
            


            // Set up the initial state for the D3DImage.
            HRESULT.Check(SetSize(512, 512));
            HRESULT.Check(SetAlpha(false));
            HRESULT.Check(SetNumDesiredSamples(4));

            

            // 
            // Optional: Subscribing to the IsFrontBufferAvailableChanged event.
            //
            // If you don't render every frame (e.g. you only render in 
            // reaction to a button click), you should subscribe to the
            // IsFrontBufferAvailableChanged event to be notified when rendered content 
            // is no longer being displayed. This event also notifies you when 
            // the D3DImage is capable of being displayed again. 

            // For example, in the button click case, if you don't render again when 
            // the IsFrontBufferAvailable property is set to true, your 
            // D3DImage won't display anything until the next button click.
            //
            // Because this application renders every frame, there is no need to
            // handle the IsFrontBufferAvailableChanged event.
            // 
            CompositionTarget.Rendering += new EventHandler(CompositionTarget_Rendering);

            //
            // Optional: Multi-adapter optimization
            //
            // The surface is created initially on a particular adapter.
            // If the WPF window is dragged to another adapter, WPF
            // ensures that the D3DImage still shows up on the new
            // adapter. 
            //
            // This process is slow on Windows XP.
            //
            // Performance is better on Vista with a 9Ex device. It's only 
            // slow when the D3DImage crosses a video-card boundary.
            //
            // To work around this issue, you can move your surface when
            // the D3DImage is displayed on another adapter. To
            // determine when that is the case, transform a point on the
            // D3DImage into screen space and find out which adapter
            // contains that screen space point.
            //
            // When your D3DImage straddles two adapters, nothing  
            // can be done, because one will be updating slowly.
            //
            _adapterTimer = new DispatcherTimer();
            _adapterTimer.Tick += new EventHandler(AdapterTimer_Tick);
            _adapterTimer.Interval = new TimeSpan(0, 0, 0, 0, 500);
            _adapterTimer.Start();

            //
            // Optional: Surface resizing
            //
            // The D3DImage is scaled when WPF renders it at a size 
            // different from the natural size of the surface. If the
            // D3DImage is scaled up significantly, image quality 
            // degrades. 
            // 
            // To avoid this, you can either create a very large
            // texture initially, or you can create new surfaces as
            // the size changes. Below is a very simple example of
            // how to do the latter.
            //
            // By creating a timer at Render priority, you are guaranteed
            // that new surfaces are created while the element
            // is still being arranged. A 200 ms interval gives
            // a good balance between image quality and performance.
            // You must be careful not to create new surfaces too 
            // frequently. Frequently allocating a new surface may 
            // fragment or exhaust video memory. This issue is more 
            // significant on XDDM than it is on WDDM, because WDDM 
            // can page out video memory.
            //
            // Another approach is deriving from the Image class, 
            // participating in layout by overriding the ArrangeOverride method, and
            // updating size in the overriden method. Performance will degrade
            // if you resize too frequently.
            //
            // Blurry D3DImages can still occur due to subpixel 
            // alignments. 
            //
            _sizeTimer = new DispatcherTimer(DispatcherPriority.Render);
            _sizeTimer.Tick += new EventHandler(SizeTimer_Tick);
            _sizeTimer.Interval = new TimeSpan(0, 0, 0, 0, 200);
            _sizeTimer.Start();
        }

        ~MainWindow()
        {
            Destroy();
        }

        void AdapterTimer_Tick(object sender, EventArgs e)
        {
            POINT p = new POINT(imgelt.PointToScreen(new Point(0, 0)));

            HRESULT.Check(SetAdapter(p));
        }

        void SizeTimer_Tick(object sender, EventArgs e)
        {
            // The following code does not account for RenderTransforms.
            // To handle that case, you must transform up to the root and 
            // check the size there.

            // Given that the D3DImage is at 96.0 DPI, its Width and Height 
            // properties will always be integers. ActualWidth/Height 
            // may not be integers, so they are cast to integers. 
            uint actualWidth = (uint)imgelt.ActualWidth;
            uint actualHeight = (uint)imgelt.ActualHeight;
            if ((actualWidth > 0 && actualHeight > 0) &&
                (actualWidth != (uint)d3dimg.Width || actualHeight != (uint)d3dimg.Height))
            {
                HRESULT.Check(SetSize(actualWidth, actualHeight));
            }
        }

        void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            RenderingEventArgs args = (RenderingEventArgs)e;

            // It's possible for Rendering to call back twice in the same frame 
            // so only render when we haven't already rendered in this frame.
            if (d3dimg.IsFrontBufferAvailable && _lastRender != args.RenderingTime)
            {
                IntPtr pSurface = IntPtr.Zero;
                HRESULT.Check(GetBackBufferNoRef(out pSurface));
                if (pSurface != IntPtr.Zero)
                {
                    /*
                    d3dimg.Lock();
                    // Repeatedly calling SetBackBuffer with the same IntPtr is 
                    // a no-op. There is no performance penalty.
                    d3dimg.SetBackBuffer(D3DResourceType.IDirect3DSurface9, pSurface);
                    HRESULT.Check(Render());
                    d3dimg.AddDirtyRect(new Int32Rect(0, 0, d3dimg.PixelWidth, d3dimg.PixelHeight));
                    d3dimg.Unlock();
                    */
                    _lastRender = args.RenderingTime;

                    m_HControl.Render(m_selectNode);
                    //m_HControl2.Render();
                }
            }
        }

        DispatcherTimer _sizeTimer;
        DispatcherTimer _adapterTimer;
        TimeSpan _lastRender;

        // Import the methods exported by the unmanaged Direct3D content.

        [DllImport("D3DContent.dll")]
        static extern int GetBackBufferNoRef(out IntPtr pSurface);

        [DllImport("D3DContent.dll")]
        static extern int SetSize(uint width, uint height);

        [DllImport("D3DContent.dll")]
        static extern int SetAlpha(bool useAlpha);

        [DllImport("D3DContent.dll")]
        static extern int SetNumDesiredSamples(uint numSamples);

        [StructLayout(LayoutKind.Sequential)]
        struct POINT
        {
            public POINT(Point p)
            {
                x = (int)p.X;
                y = (int)p.Y;
            }

            public int x;
            public int y;
        }

        [DllImport("D3DContent.dll")]
        static extern int SetAdapter(POINT screenSpacePoint);

        [DllImport("D3DContent.dll")]
        static extern int Render();

        [DllImport("D3DContent.dll")]
        static extern void Destroy();

        [DllImport("D3DContent.dll")]
        static extern void SetCamera(POINT pos);

        [DllImport("user32.dll")]
        public static extern int GetKeyboardState(byte[] keystate);
        /*
        private void Grid_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            mouseDownEvent = true;
            m_oldMouseX = (int)e.GetPosition(this.imgelt).X;
            m_oldMouseY = (int)e.GetPosition(this.imgelt).Y;
        }

        private void Grid_MouseMove(object sender, MouseEventArgs e)
        {
            if (mouseDownEvent == true)
            {
                int xCooldinate = (int)e.GetPosition(this.imgelt).X;
                int yCooldinate = (int)e.GetPosition(this.imgelt).Y;
                //label1.Content = xCooldinate.ToString() + "," +yCooldinate.ToString();
                POINT pos = new POINT();
                pos.x = xCooldinate - m_oldMouseX;
                pos.y = yCooldinate - m_oldMouseY;
                SetCamera(pos);

                m_oldMouseX = xCooldinate;
                m_oldMouseY = yCooldinate;

            }
        }

        private void Grid_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            mouseDownEvent = false;
        }
        */
        private void hostWinForm_KeyDown(object sender, KeyEventArgs e)
        {
            /*
            if (e.Key == Key.A)
            {
                m_HControl.m_HEngine.MoveXAxis(-4.0f);
            }
            if (e.Key == Key.D)
            {
                m_HControl.m_HEngine.MoveXAxis(4.0f);
            }
            if (e.Key == Key.W)
            {
                m_HControl.m_HEngine.MoveZAxis(4.0f);
            }
            if (e.Key == Key.S)
            {
                m_HControl.m_HEngine.MoveZAxis(-4.0f);
            }
             */
            /*
            byte[] keys = new byte[255];

            GetKeyboardState(keys);

            if (keys[(int)Key.A] == 129)
            {
                m_HControl.m_HEngine.MoveXAxis(-4.0f);
            }

            if (keys[(int)Key.D] == 129)
            {
                m_HControl.m_HEngine.MoveXAxis(4.0f);
            }

            if (keys[(int)Key.W] == 129)
            {
                m_HControl.m_HEngine.MoveZAxis(4.0f);
            }

            if (keys[(int)Key.S] == 129)
            {
                m_HControl.m_HEngine.MoveZAxis(-4.0f);
            }
             */

        }

        private void radioDraw_Checked(object sender, RoutedEventArgs e)
        {
            m_HControl.AlphaErase = false;
        }

        private void radioErase_Checked(object sender, RoutedEventArgs e)
        {
            m_HControl.AlphaErase = true;
        }

        private void ChangeTexture(string strTex)
        {
            BitmapImage myBitmapImage = new BitmapImage();
            myBitmapImage.BeginInit();
            myBitmapImage.UriSource = new Uri(@strTex);
            myBitmapImage.EndInit();
            //m_HControl.m_HEngine.GetTexturePath(AlphaMapList.SelectedIndex);
            Image_SelectedTexture.Source = myBitmapImage;
        }

        private void AlphaMapList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            //ListBoxItem lbi = (sender as System.Windows.Controls.ListBox).SelectedItem as ListBoxItem;
            m_HControl.AlphaSelectedIndex = AlphaMapList.SelectedIndex;
            
            ChangeTexture( m_HControl.m_HEngine.GetTexturePath(AlphaMapList.SelectedIndex) );

            AlphaTextureInfo.SelectedObject = m_alphaprop[AlphaMapList.SelectedIndex];
        }

        

        // CustomSlider에서 값이 변경 되었다~  
        private void TexturePathChanged(object sender, PropertyChangedEventArgs e)  
        {  
            if (e != null)  
            {
                m_HControl.m_HEngine.SetTexturePath(AlphaMapList.SelectedIndex, m_alphaprop[AlphaMapList.SelectedIndex].TexturePath );
                ChangeTexture(m_HControl.m_HEngine.GetTexturePath(AlphaMapList.SelectedIndex));
            }  
        }

        private void scenodeTree_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            TreeView treeView = sender as TreeView;
            

            if (null != treeView)
            {
                
                    string selectedItemName = treeView.SelectedItem as string;
                    if (selectedItemName == null)
                    {
                        TreeViewItem _selectedItem = (TreeViewItem)treeView.SelectedItem;
                        if (_selectedItem.Header == "TileTerrain")
                        {
                            m_selectNode = "TileTerrain";
                        }
                        else
                        {
                            m_selectNode = "";
                        }
                    }
                    else
                    {
                        m_selectNode = selectedItemName;
                    }
               
            }
        }

        private void ListBoxItem_Selected(object sender, RoutedEventArgs e)
        {
            
            m_HControl.m_HEngine.SetPVSObjectNum(scenodeList.SelectedIndex);
        }

        private void FileMenuItem_Click(object sender, RoutedEventArgs e)
        {
            MenuItem mnu = (MenuItem)e.OriginalSource;
            string path;
            switch (mnu.Name) 
            {
                case "New":
                    // Undo 큐에 작업내용 있을시에는 저장 물어본 후에 OK하면 저장후 새로생성

                    // 생성
                    TerrainCreateDialog newTerrainDialog = new TerrainCreateDialog();
                    newTerrainDialog.Owner = this;
                    newTerrainDialog.ShowDialog();
                    if (newTerrainDialog.DialogResult.HasValue && newTerrainDialog.DialogResult.Value)
                    {
                        m_HControl.m_HEngine.NewTerrain
                            (
                                newTerrainDialog.i_scale,
                                newTerrainDialog.i_heightscale,
                                newTerrainDialog.i_tilenum,
                                newTerrainDialog.i_vertexnum
                                );
                    }


                    
                    break;
                case "Save":
                    path = TerrainManager.SaveTerrain();
                    if (path != string.Empty)
                    {
                        m_HControl.m_HEngine.SaveTerrain(path);
                    }
                    break;
                case "Load":
                    path = TerrainManager.OpenTerrain();
                    if (path != string.Empty)
                    {
                        m_HControl.m_HEngine.LoadTerrain(path);
                    }
                    break;
                case "Exit":
                    // Undo 큐에 작업내용 있을시에는 물어본 후에 종료

                    //종료
                    Application.Current.Shutdown();
                    break;
            }
        }

        private void WIREMODE_Click(object sender, RoutedEventArgs e)
        {
            m_HControl.m_HEngine.m_bWireframe = (bool)WIREMODE.IsChecked;
        }

        private void TabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (this.TerrainEditMode.SelectedIndex ==0) //alphaMap
            {
                m_HControl.m_TerrainEditMode =0;
            }
            else
            {
                m_HControl.m_TerrainEditMode =1;
            }
        }

        private void radioUp_Checked(object sender, RoutedEventArgs e)
        {
            m_HControl.m_HEngine.SetHeightEditMode(0);
        }

        private void radioDown_Checked(object sender, RoutedEventArgs e)
        {
            m_HControl.m_HEngine.SetHeightEditMode(1);
        }

        private void radioFlat_Checked(object sender, RoutedEventArgs e)
        {
            m_HControl.m_HEngine.SetHeightEditMode(2);
        }

        private void radioFlatLand_Checked(object sender, RoutedEventArgs e)
        {
            m_HControl.m_HEngine.SetHeightEditMode(3);
        }

        private void LODLEVEL_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            foreach (char c in e.Text)
            {
                if (e.Text !="." && !char.IsDigit(c))
                {
                    e.Handled = true;
                    break;
                }
            }
        }

        private void LODLEVEL_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                m_HControl.m_HEngine.SetLODrate(float.Parse(this.LODLEVEL.Text));
            }
        }

       


    }

 

    public static class HRESULT
    {
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        public static void Check(int hr)
        {
            Marshal.ThrowExceptionForHR(hr);
        }
    }

}
