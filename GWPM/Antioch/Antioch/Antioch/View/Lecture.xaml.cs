﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Antioch.View
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Lecture : ContentView
    {
        public Lecture()
        {
            InitializeComponent();
        }
    }
}