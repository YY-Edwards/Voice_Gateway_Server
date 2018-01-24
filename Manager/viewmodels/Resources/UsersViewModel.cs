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
using System.Windows.Data;

using Sigmar;

using Manager.Models;

namespace Manager.ViewModels
{
    
    public class UsersViewModel:ResourcesViewModel<User>
    {
        public UsersViewModel()
        {
            _resourcesName = Resource.User;
            ResourceOpcode = RequestOpcode.user;
            RecordsName = "users";

            Password = string.Empty;
        }

        public string UserName { set { Element.UserName = value; } get { return Element.UserName; } }
        public string Password { set { Element.Password = value; } get { return Element.Password; } }
        public int TypeIndex { set { Element.Type = value == 1 ? User.UserType.admin : User.UserType.guest; } get { return Element.Type == User.UserType.admin ? 1 : 0; } }


        protected override void CreateElement()
        {
            if (UserName != null && UserName != "") base.CreateElement();
        }
        protected override void OnResourceChanged()
        {          
        }
   
        protected override void OnSelectedChanged()
        {
            if (Element != null) NotifyPropertyChanged(new string[]{
                "UserName"
                ,"TypeIndex"          
            });
        }        
    }
}
