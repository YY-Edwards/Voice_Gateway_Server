using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Controls;
using System.ComponentModel;
using System.Threading;
using System.Windows.Media.Imaging;
using System.IO;

using Sigmar;
using Manager.Models;
namespace Manager.ViewModels
{
    public class AreaViewModel : ResourcesViewModel<Area>
    {
        private List<Area> _uploadImages;

        public AreaViewModel()
        {
            _resourcesName = Resource.Areas;
            ResourceOpcode = RequestOpcode.area;
            RecordsName = "areas";

            _uploadImages = new List<Area>();

            ExtName = string.Empty;
        }

        public List<Area> AreaList { get { return _elements; } }
        public string ExtName { set; get; }

        public string Name { set { Element.Name = value; } get { if (Element == null)return null; return Element.Name; } }  
        public string MapUrl
        {
            get
            {
                if(Element.IsLocal)
                {
                    return Element.LocalPath;
                }
                else
                {
                    return Element.Map;
                }
            }

            set
            {
                if(value != null && File.Exists(value))
                {
                    Element.IsLocal = true;
                    Element.LocalPath = value;
                    Element.MapName = DateTime.Now.Ticks.ToString() + ExtName;
                    Element.Map = HttpServer.Instance().ImagesUrl + Element.MapName;

                    try
                    {
                        BitmapImage mapImage = new BitmapImage(new Uri(value));
                        Element.Width = mapImage.Width;
                        Element.Height = mapImage.Height;
                    }
                    catch
                    {

                    }
                }
            }          
        }

        protected override void OnResourceChanged()
        {
            NotifyPropertyChanged("AreaList");
        }

        protected override void OnSelectedChanged()
        {
            if (Element != null) NotifyPropertyChanged(new string[]{
                "Name"
                ,"MapUrl"          
            });
        }        

        public override SaveStatus Save()
        {
            if (_uploadImages != null)
            {
                foreach(var element in _uploadImages)
                {
                    if (element.IsLocal)
                    {
                        HttpServer.Instance().Upload(element.MapName, element.LocalPath);
                        element.IsLocal = false;

                        if (element == Element) OnSelectedChanged();
                    }
                }
            }
            


            SaveStatus reslut = base.Save() == SaveStatus.Failure ? SaveStatus.Failure : SaveStatus.Success;
            return reslut;
        }


        new public ICommand Create
        {
            get
            {
                return new Command(() =>
                {
                    base.Create.Execute(null);

                    if (Element != null && Element.IsLocal)
                    {
                        if (!_uploadImages.Contains(Element))
                        {
                            _uploadImages.Add(Element);
                        }
                    }
                });
            }
        }

        new public ICommand Modify
        {
            get
            {
                return new Command(() =>
                {
                    base.Modify.Execute(null);

                    if (Element != null && Element.IsLocal)
                    {
                        if (!_uploadImages.Contains(Element))
                        {
                            _uploadImages.Add(Element);
                        }
                    }
                });
            }
        }
    }
}
