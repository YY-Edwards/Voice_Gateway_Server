﻿using System;
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

namespace TrboX
{
    /// <summary>
    /// TesTitle.xaml 的交互逻辑
    /// </summary>
    public partial class TesTitle : Window
    {
        public TesTitle()
        {
            InitializeComponent();
            Loaded += delegate
            {
                Title = "Hello";
            };
        }
    }
}
