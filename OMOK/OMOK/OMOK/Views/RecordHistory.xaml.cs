﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace OMOK.Views
{
    public class Record
    {
        public string Text { get; set; }
    }

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class RecordHistory : ContentPage
    {
        public RecordHistory()
        {
            InitializeComponent();

            leaveButton.Clicked += (sender, e) => {
                Navigation.PopModalAsync();
            };
    
            var list = SQLite.ReadResultLog();

            var strlist = new Record[list.Count];
            int pos = 0;
            foreach (var r in list)
            {
                int index = pos + 1;
                var text = index.ToString() + ". " + r.Time.ToString("yy-MM-dd HH:mm") + " " + r.MyName + " vs " + r.OpponentName + (r.Result == 1 ? " 승리" : " 패배");
                text.Replace('\n', ' ');
                strlist[pos++] = new Record() { Text = text };
            }

            listview.SeparatorColor = Color.Black;

            listview.ItemsSource = strlist;
            
        }
    }
}