using System;
using System.Threading;
using WBA.Network;

using NetClient;
using WBA.MainTabbedPage;
using Xamarin.Forms;
using WBA.Navigation;
using System.Linq;

namespace WBA
{
    public partial class MainPage : TabbedPage
    {
        public static Page GetPageByTitle(MainPage page,string name)
        {
            foreach (var child in page.Children)
            {
                if (child.Title == name)
                    return child;
            }

            return null;
        }

        protected override void OnDisappearing()
        {
            base.OnDisappearing();
        }

        public Page ChatPage = null;

        public MainPage()
        {
            var setting = new Setting();
            var community = new Community();

            Children.Add(new Notice());
            Children.Add(new Bible());
            Children.Add(new Planxaml());
            Children.Add(new PlanDetail());
            Children.Add(community);
            Children.Add(setting);

           
                //네트워크 메세지 수신
                MessagingCenter.Subscribe<CompletePacket>(

                this, "recv_packet", (data) =>
                {

                    switch (data.Protocol)
                    {
                        case (int)PROTOCOL.IdPktVersionRes:
                            {
                                VERSION_RES res = new VERSION_RES();
                                res = VERSION_RES.Parser.ParseFrom(data.Data);
                            }
                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                MessagingCenter.Send<Setting, CompletePacket>(setting, "setting", data);
                            }
                            break;
                        case (int)PROTOCOL.IdPktCreateRoomRes:
                            {
                                CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                                res = CREATE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        ChatPage = new MainChatView();
                                        Navigation.PushModalAsync(new NavigationPage(ChatPage));
                                    });
                                }

                            }
                            break;
                        case (int)PROTOCOL.IdPktNewUserInRoomNty:
                            {
                                NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);
                            }
                            break;
                        case (int)PROTOCOL.IdPktBroadcastRoomMessageRes:
                            {
                               if(ChatPage != null)
                                {
                                    MessagingCenter.Send<MainChatView, CompletePacket>((MainChatView)ChatPage, "maintchatview", data);
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomListRes:
                            {
                                MessagingCenter.Send<Community, CompletePacket>(community, "community", data);
                            }
                            break;
                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);
                            }
                            break;
                    }

                });
           

        }


    }
}
