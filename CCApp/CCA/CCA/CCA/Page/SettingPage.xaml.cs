using DependencyHelper;
using Rg.Plugins.Popup.Pages;
using Rg.Plugins.Popup.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace CCA.Page
{

    //MODEL
    class SettingViewModel : INotifyPropertyChanged
    {
        private string _ImageUrl { get; set; }

        public string ImageUrl
        {
            get { return _ImageUrl; }
            set
            {
                _ImageUrl = value;
                OnPropertyChanged(nameof(ImageUrl));
            }
        }

        private string _Name { get; set; }

        public string Name
        {
            get { return _Name; }
            set
            {
                _Name = value;
                OnPropertyChanged(nameof(Name));
            }
        }

        private string _State { get; set; }

        public string State
        {
            get { return _State; }
            set
            {
                _State = value;
                OnPropertyChanged(nameof(State));
            }
        }

        private string _EMail { get; set; }

        public string EMail
        {
            get { return _EMail; }
            set
            {
                _EMail = value;
                OnPropertyChanged(nameof(EMail));
            }
        }

        private string _UID { get; set; }

        public string UID
        {
            get { return _UID; }
            set
            {
                _UID = value;
                OnPropertyChanged(nameof(UID));
            }
        }

        private string _Version { get; set; }

        public string Version
        {
            get { return _Version; }
            set
            {
                _Version = value;
                OnPropertyChanged(nameof(Version));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SettingPage : PopupPage
    {
        SettingViewModel vm = new SettingViewModel();
        private double StepValue = 1.0;
        public SettingPage()
        {
            InitializeComponent();

            vm.UID = "0";
            vm.Version = VersionTracking.CurrentVersion;

            {
                var currentVersion = VersionTracking.CurrentVersion;

                double myversion = 0;
                double.TryParse(currentVersion,out myversion);
                const Double Eps = 0.000000000000001;

                if (Math.Abs(User.Version - myversion) > Eps)
                {
                    vm.Version = "_" + vm.Version;
                }
            }

            //이미 로그인 중이면 메세지 표시를 변경
           
                Device.BeginInvokeOnMainThread(() =>
                {
                    if (User.LoginSuccess == true)
                    {
                        State.Text = "접속중";
                        vm.UID = "UID : " + User.Uid;
                        vm.EMail = User.EMail;
                        vm.Name = User.Name;
                        vm.ImageUrl = User.ProfileUrl;
                    }
                    else
                    {
                        State.Text = "DisConncted";
                    }

                });
               
          
            BindingContext = vm;
        }

        async void OnCheckVersion(object sender, System.EventArgs e)
        {
            var currentVersion = VersionTracking.CurrentVersion;

            double myversion;
            double.TryParse(currentVersion,out myversion);
            const Double Eps = 0.000000000000001;

            if (Math.Abs(User.Version - myversion) > Eps)
            {
                Xamarin.Essentials.Browser.OpenAsync("https://play.google.com/store/apps/details?id=antioch.kor.pkg");
                return;
            }
        }

        private async void OnCloseButtonClicked(object sender, EventArgs e)
        {
            await PopupNavigation.Instance.PopAsync();
        }

        private async void OnLogoutButtonClicked(object sender, EventArgs e)
        {
            if (PopupNavigation.Instance.PopupStack.Count > 0)
                await PopupNavigation.Instance.PopAsync();

            DependencyService.Get<MethodExt>().Logout();
        }

    }
}