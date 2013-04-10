using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Media.Imaging;
using System.Activities.Presentation.PropertyEditing;


namespace D3DHost
{
    class AlphaMapInfo : INotifyPropertyChanged 
    {
        //public enum Gender { Male, Female }  
  
        private string m_name;  
        private int m_index;  
        private string m_TexturePath;
        //private Gender m_eGender;

        #region INotifyPropertyChanged Members  
        public event PropertyChangedEventHandler PropertyChanged;  
        private void OnPropertyChanged(String info)  
        {  
            if (PropertyChanged != null)  
            {  
                PropertyChanged(this, new PropertyChangedEventArgs(info));  
            }  
        }  
        #endregion  


        public AlphaMapInfo()  
        {
            
        }

        [Category("General")]
        [Description("이름")]
        public string Name  
        {
            set { m_name = value; }
            get { return m_name; }  
        }

        [Category("General")]
        [Description("인덱스")]
        public int Index
        {
            set { m_index = value; }
            get { return m_index; }  
        }  
        /*
        public Gender genderProp  
        {  
            set { m_eGender = value; }  
            get { return m_eGender; }  
        }
        */
        [Category("General")]
        [Description("텍스쳐변경")]
        [Editor(typeof(TextureEditor), typeof(PropertyValueEditor))]
        public string TexturePath 
        { 
            get
            {
                return m_TexturePath;
            }
            set
            {
                m_TexturePath = value;
                OnPropertyChanged("Value");
            }
        }


        public void SetData(int index, string strTex)
        {
            Index = index;
            TexturePath = strTex;
        }
    }
}
