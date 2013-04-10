using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace D3DHost
{
    class TerrainManager
    {
        internal static string OpenTerrain()
        {
            OpenFileDialog dialog = new OpenFileDialog();

            dialog.CheckFileExists = true;
            dialog.DefaultExt = ".terrain";
            dialog.Filter = "Picture Files (*.terrain)|*.terrain;";
            dialog.Multiselect = false;
            dialog.Title = "Terrain File";

            DialogResult dr = dialog.ShowDialog();
            if (dr==DialogResult.OK)
            {
                try
                {

                    return dialog.FileName;
                    
                }
                catch {
                
                }
            }

            return string.Empty;
        }
        

        internal static string SaveTerrain()
        {
            SaveFileDialog dialog = new SaveFileDialog();

            dialog.CreatePrompt = true;
            dialog.OverwritePrompt = true;
            dialog.DefaultExt = ".terrain";
            dialog.Filter = "Picture Files (*.terrain)|*.terrain;";
            dialog.Title = "Terrain File";
            dialog.InitialDirectory = @"..";

            DialogResult dr = dialog.ShowDialog();
            if (dr==DialogResult.OK)
            {
                try
                {

                    return dialog.FileName;
                    
                }
                catch {
                
                }
            }

            return string.Empty;
        }

        
    }
}
