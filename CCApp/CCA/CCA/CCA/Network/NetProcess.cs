using Google.Protobuf;
using System;
using System.IO;
using Xamarin.Essentials;
using System.Text;
using System.Net;
using System.Threading;
using System.Collections.Concurrent;
using Xamarin.Forms;
using System.Linq;
using DependencyHelper;
using Rg.Plugins.Popup.Services;
using CCA.Page;
using System.Collections.Generic;

namespace CCA
{
    public static class NetProcess
    {

        static public bool IsActivate = false;

        static public Client client = new Client();

        public static string GetIPAddress(string hostname)
        {
            IPHostEntry host;
            host = Dns.GetHostEntry(hostname);

            foreach (IPAddress ip in host.AddressList)
            {
                if (ip.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                {
                    //System.Diagnostics.Debug.WriteLine("LocalIPadress: " + ip);
                    return ip.ToString();
                }
            }
            return string.Empty;
        }

        private static DateTime check_time = new DateTime();

        private static DateTime notice_time = new DateTime();

        static public void start()
        {
            if (check_time < DateTime.Now)
            {
                //string ip = "192.168.0.9";

                string ip = GetIPAddress("blessnhs.iptime.org");

                client.StartClient3(ip, 20000);

                check_time = DateTime.Now.AddSeconds(10);
            }

            if (notice_time < DateTime.Now)
            {

                  SendMailList();

                //SendAlaram();

                //SendRoomList();

                notice_time = DateTime.Now.AddSeconds(60 * 1);
            }
        }

        public static ConcurrentQueue<StreamWrapper> JpegStream = new ConcurrentQueue<StreamWrapper>();
        public static ConcurrentQueue<StreamWrapper> AudioStream = new ConcurrentQueue<StreamWrapper>();

        public static ConcurrentQueue<StreamWrapper> Mpeg2Stream = new ConcurrentQueue<StreamWrapper>();
        public static List<long> TargetPlayerId = new List<long>();
        public static int TargetBatteryGage;

        static public void Loop()
        {

            if (client.socket == null || client.socket.Connected == false)
                return;

            CompletePacket data;
            while (client.PacketQueue.TryDequeue(out data) == true)
            {
                if (data == null) return;

                try
                {
                    switch (data.Protocol)
                    {
                        case (int)PROTOCOL.IdPktVersionRes:
                            {
                                VERSION_RES res = new VERSION_RES();
                                res = VERSION_RES.Parser.ParseFrom(data.Data);

                                var currentVersion = VersionTracking.CurrentVersion;

                                double myversion;
                                if(double.TryParse(currentVersion,out myversion) == false)
                                { 
                                }

                                const Double Eps = 0.000000000000001;

                                User.Version = res.VarVersion;

                                if (Math.Abs(res.VarVersion - myversion) > Eps)
                                {
                                    //첫 검수라 임시 주석 2번째 패치 부터는 활성화
                                    if (Device.RuntimePlatform == Device.Android && User.OnceVersionNotify == false)
                                    {
                                        Device.BeginInvokeOnMainThread(() =>
                                        {
                                            User.OnceVersionNotify = true;
                                            DependencyService.Get<MethodExt>().Notification("New Version Updated");
                                        });
                                    }
                                }

                                SQLLiteDB.LoadCacheData();


                                //계정 변환중이면 처리안한다.
                                if (User.isSwitching == true)
                                    return;

                                //구글 인증 성공하고 게임서버에 한번도 로그인을 하지않으면 로그인시도한다.
                                if (User.Name != null && User.DBId == 0 )
                                    NetProcess.SendLogin(User.Uid,User.Token);
                                else if(User.DBId == 0)
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        DependencyService.Get<MethodExt>().RestartApp();
                                    });
                                }
                                else if (User.DBId != 0)
                                {

                                    var machineid = DependencyService.Get<MethodExt>().MachineId();
                                    var machineName = DependencyService.Get<MethodExt>().MachineName();

                                    NetProcess.Recoonect(machineid, machineName, User.Token, User.DBId, User.EMail, User.Name);
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktReconnectRes:
                            {
                                RECONNECT_RES res = new RECONNECT_RES();
                                res = RECONNECT_RES.Parser.ParseFrom(data.Data);
                                if (res.VarCode == ErrorCode.Success)
                                {
                                    TargetPlayerId.Clear();
                                    JpegStream = new ConcurrentQueue<StreamWrapper>();
                                    AudioStream = new ConcurrentQueue<StreamWrapper>();

                                    User.LoginSuccess = true;
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktLoginRes:
                            {
                                LOGIN_RES res = new LOGIN_RES();
                                res = LOGIN_RES.Parser.ParseFrom(data.Data);

                                var mainpage = (MainPage)Application.Current.MainPage;

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    Xamarin.Forms.DependencyService.Register<MethodExt>();
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        DependencyService.Get<MethodExt>().ShowToast("로그인 하셨습니다.");

                                        while (PopupNavigation.Instance.PopupStack.Count > 0)
                                        {
                                            PopupNavigation.Instance.PopAsync();
                                        }
                                    });

                                    User.LoginSuccess = true;

                                    User.DBId = res.VarIndex;
                                    User.EMail = res.VarEmail;
                                    SQLLiteDB.Upsert(res.VarName,"");

                                    TargetPlayerId.Clear();
                                    JpegStream = new ConcurrentQueue<StreamWrapper>();
                                    AudioStream = new ConcurrentQueue<StreamWrapper>();

                                   
                                }   
                                else
                                {  
                                    Mpeg2Stream = new ConcurrentQueue<StreamWrapper>();
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        DependencyService.Get<MethodExt>().RestartApp();
                                    });
                                }

                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomListRes:
                            {

                                ROOM_LIST_RES res = new ROOM_LIST_RES();
                                res = ROOM_LIST_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktNoticeRes:
                            {

                                NOTICE_RES res = new NOTICE_RES();
                                res = NOTICE_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var page = new NoticePage();
                                    page.LoadList(res);
                                    PopupNavigation.Instance.PushAsync(page);


                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktRegCameraRes:
                            {

                                REG_CAMERA_RES res = new REG_CAMERA_RES();
                                res = REG_CAMERA_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    if (res.VarCode == ErrorCode.Success)
                                    {
                                        DependencyService.Get<MethodExt>().ShowToast("카메라 등록에 성공했습니다.");
                                    }
                                    else
                                    {
                                        DependencyService.Get<MethodExt>().ShowToast("카메라 등록에 실패했습니다.");
                                    }
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktCameraListRes:
                            {

                                CAMERA_LIST_RES res = new CAMERA_LIST_RES();
                                res = CAMERA_LIST_RES.Parser.ParseFrom(data.Data);

                                while (PopupNavigation.Instance.PopupStack.Count > 0)
                                {
                                    PopupNavigation.Instance.PopAsync();
                                }

                                if (res.VarCode == ErrorCode.Success)
                                {
                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        var mainpage = (MainPage)Application.Current.MainPage;

                                        PopupNavigation.Instance.PushAsync(new CameraListPage(res.VarCamera));

                                    });
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktMailListRes:
                            {

                                MAIL_LIST_RES res = new MAIL_LIST_RES();
                                res = MAIL_LIST_RES.Parser.ParseFrom(data.Data);

                            
                            }
                            break;
                        case (int)PROTOCOL.IdPktPrayMessageRes:
                            {

                               
                            }
                            break;
                        case (int)PROTOCOL.IdPktCameraWakeUpRes:
                            {
                                CAMERA_WAKE_UP_RES res = new CAMERA_WAKE_UP_RES();
                                res = CAMERA_WAKE_UP_RES.Parser.ParseFrom(data.Data);

                                var machineid = DependencyService.Get<MethodExt>().MachineId();

                                if (res.VarMachineId == machineid)
                                {

                                    TargetPlayerId.Add(res.VarToPlayerId);

                                    Device.BeginInvokeOnMainThread(() =>
                                    {
                                        if (PopupNavigation.Instance.PopupStack.Count == 0)
                                        {
                                            PopupNavigation.Instance.PushAsync(new CameraPage());
                                        }
                                        else
                                        {
                                            if (PopupNavigation.Instance.PopupStack[0].GetType() != typeof(CameraPage))
                                            {
                                                PopupNavigation.Instance.PopAsync();

                                                PopupNavigation.Instance.PushAsync(new CameraPage());
                                            }
                                        }
                                    });
                                }
                                else
                                {
                                    PopupNavigation.Instance.PushAsync(new CameraViewer(res.VarMachineId, res.VarToPlayerId,null));
                                }
                            }
                            break;

                        case (int)PROTOCOL.IdPktStopStreamRes:
                            {
                                STOP_STREAM_RES res = new STOP_STREAM_RES();
                                res = STOP_STREAM_RES.Parser.ParseFrom(data.Data);

                                TargetPlayerId.Remove(res.VarToPlayerId);

                            }
                            break;

                        case (int)PROTOCOL.IdPktCreateRoomRes:
                            {
                                CREATE_ROOM_RES res = new CREATE_ROOM_RES();
                                res = CREATE_ROOM_RES.Parser.ParseFrom(data.Data);

                                if (res.VarCode != ErrorCode.Success)
                                    break;

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;

                                    User.RoomIdList.Add(res.VarRoomId);
                                });

                            }
                            break;
                        case (int)PROTOCOL.IdPktEnterRoomRes:
                            {
                                ENTER_ROOM_RES res = new ENTER_ROOM_RES();
                                res = ENTER_ROOM_RES.Parser.ParseFrom(data.Data);
                                
                                if (res.VarCode != ErrorCode.DuplicateEnterRoom && res.VarCode != ErrorCode.Success)
                                    break;

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    try
                                    {
                                        var mainpage = (MainPage)Application.Current.MainPage;
                                    }
                                    catch (Exception e)
                                    {

                                    }
                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktRoomPassThroughRes:
                            {
                                ROOM_PASS_THROUGH_RES res = new ROOM_PASS_THROUGH_RES();
                                res = ROOM_PASS_THROUGH_RES.Parser.ParseFrom(data.Data);
                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    var mainpage = (MainPage)Application.Current.MainPage;


                                });
                            }
                            break;
                        case (int)PROTOCOL.IdPktLeaveRoomRes:
                            {
                                LEAVE_ROOM_RES res = new LEAVE_ROOM_RES();
                                res = LEAVE_ROOM_RES.Parser.ParseFrom(data.Data);

                                var mainpage = (MainPage)Application.Current.MainPage;

                                Device.BeginInvokeOnMainThread(() =>
                                {

                                });
                            }
                            break;

                        case (int)PROTOCOL.IdPktAudioMessageRes:
                            {
                                AUDIO_MESSAGE_RES res = new AUDIO_MESSAGE_RES();
                                res = AUDIO_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    StreamWrapper wra = new StreamWrapper();
                                    wra.stream = new MemoryStream(msg.ToByteArray());
                                    wra.pos = res.VarPos;
                                    AudioStream.Enqueue(wra);
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktBitmapMessageRes:
                            {
                                BITMAP_MESSAGE_RES res = new BITMAP_MESSAGE_RES();
                                res = BITMAP_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    StreamWrapper wra = new StreamWrapper();
                                    wra.stream = new MemoryStream(msg.ToByteArray());
                                    wra.width = res.VarWidth;
                                    wra.height = res.VarHeight;
                                    JpegStream.Enqueue(wra);
                                }
                            }
                            break;
                        case (int)PROTOCOL.IdPktMpeg2TsMessageRes:
                            {
                                MPEG2TS_MESSAGE_RES res = new MPEG2TS_MESSAGE_RES();
                                res = MPEG2TS_MESSAGE_RES.Parser.ParseFrom(data.Data);

                                foreach (var msg in res.VarMessage)
                                {
                                    StreamWrapper wra = new StreamWrapper();
                                    wra.stream = new MemoryStream(msg.ToByteArray());
                                    wra.type = res.VarType;
                                    Mpeg2Stream.Enqueue(wra);
                                }

                            }
                            break;
                        case (int)PROTOCOL.IdPktNewUserInRoomNty:
                            {
                                NEW_USER_IN_ROOM_NTY res = new NEW_USER_IN_ROOM_NTY();
                                res = NEW_USER_IN_ROOM_NTY.Parser.ParseFrom(data.Data);

                                var mainpage = (MainPage)Application.Current.MainPage;


                            }
                            break;

                        case (int)PROTOCOL.IdPktMachineStatusRes:
                            {
                                MACHINE_STATUS_RES res = new MACHINE_STATUS_RES();
                                res = MACHINE_STATUS_RES.Parser.ParseFrom(data.Data);

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    if (PopupNavigation.Instance.PopupStack.Count > 0)
                                    {
                                        for (int i = 0; i < PopupNavigation.Instance.PopupStack.Count; i++)
                                        {
                                            if (PopupNavigation.Instance.PopupStack[i].GetType() == typeof(CameraViewer))
                                            {
                                                var page = PopupNavigation.Instance.PopupStack[i];

                                                CameraViewer camera_view = (CameraViewer)page;
                                                camera_view.TargetBatteryLevel = res.VarBattery.ToString();
                                                break;
                                            }
                                        }
                                    }
                                });
                            }
                            break;


                        case (int)PROTOCOL.IdPktCameraControlRes:
                            {
                                CAMERA_CONTROL_RES res = new CAMERA_CONTROL_RES();
                                res = CAMERA_CONTROL_RES.Parser.ParseFrom(data.Data);

                                var machineid = DependencyService.Get<MethodExt>().MachineId();

                                if (res.VarMachineId != machineid)
                                    break;

                                Device.BeginInvokeOnMainThread(() =>
                                {
                                    if (PopupNavigation.Instance.PopupStack.Count > 0)
                                    {
                                        if (PopupNavigation.Instance.PopupStack[0].GetType() == typeof(CameraPage))
                                        {
                                            var page = PopupNavigation.Instance.PopupStack[0];

                                            CameraPage camera_page = (CameraPage)page;
                                  
                                            switch (res.VarType)
                                            {
                                                case CameraControlType.SwitchCamera:
                                                    camera_page.ControlCamera("switch_camera");
                                                    break;
                                                case CameraControlType.Flash:
                                                    camera_page.ControlCamera("Flash");
                                                    break;
                                                case CameraControlType.Mic:
                                                    camera_page.ControlCamera("MIC");
                                                    break;
                                            }

                                        }
                                    }
                                });
                            }
                            break;
                    }
                }
                catch (Exception ex)
                {
                    DependencyService.Get<MethodExt>().Notification(ex.StackTrace.ToString());
                }

                data = null;
            }


            data = null;
        }

        static public void SendVersion()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            VERSION_REQ person = new VERSION_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktVersionReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

       

        static public void SendRoomBITMAPMessage(List<System.Net.Sockets.Socket> _Clients,ConcurrentQueue<System.IO.MemoryStream> list, int type, int width, int height)
        {
            {
                BITMAP_MESSAGE_REQ message = new BITMAP_MESSAGE_REQ();
                message.VarRoomNumber = User.CurrentChatViewNumber;
                message.VarType = type;
                message.VarWidth = width;
                message.VarHeight = height;

                foreach (var playerid in TargetPlayerId)
                {
                    message.VarToPlayerId.Add(playerid);
                };

                foreach (var msg in list)
                {
                    message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
                };


                foreach (var client in _Clients)
                {
                    if (client == null  || client.Connected == false)
                        return;
                    Client.WritePacket(client, (int)PROTOCOL.IdPktBitmapMessageReq, message.ToByteArray(), message.ToByteArray().Length);
                }

            }

        }
        static public void SendRoomBITMAPMessage(ConcurrentQueue<System.IO.MemoryStream> list, int type,int width,int height)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                BITMAP_MESSAGE_REQ message = new BITMAP_MESSAGE_REQ();
                message.VarRoomNumber = User.CurrentChatViewNumber;
                message.VarType = type;
                message.VarWidth = width;
                message.VarHeight = height;

                foreach (var playerid in TargetPlayerId)
                {
                    message.VarToPlayerId.Add(playerid);
                };

                foreach (var msg in list)
                {
                    message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
                };


                client.WritePacket((int)PROTOCOL.IdPktBitmapMessageReq, message.ToByteArray(), message.ToByteArray().Length);

            }

        }

        static public void SendMachineStatus(int battery)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                MACHINE_STATUS_REQ message = new MACHINE_STATUS_REQ();
           
                foreach (var playerid in TargetPlayerId)
                {
                    message.VarToPlayerId.Add(playerid);
                };

                message.VarBattery = battery;

                client.WritePacket((int)PROTOCOL.IdPktMachineStatusReq, message.ToByteArray(), message.ToByteArray().Length);

            }

        }

        static public void SendRoomMPEG2TSMessage(System.IO.MemoryStream stream, int type)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                MPEG2TS_MESSAGE_REQ message = new MPEG2TS_MESSAGE_REQ();
                message.VarRoomNumber = User.CurrentChatViewNumber;
                message.VarType = type;
                message.VarMessage.Add(ByteString.CopyFrom(stream.ToArray()));

                client.WritePacket((int)PROTOCOL.IdPktMpeg2TsMessageReq, message.ToByteArray(), message.ToByteArray().Length);

            }

        }

        static public void SendAudioMessage(ConcurrentQueue<System.IO.MemoryStream> list)
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                AUDIO_MESSAGE_REQ message = new AUDIO_MESSAGE_REQ();
                message.VarRoomNumber = User.CurrentChatViewNumber;

                foreach (var playerid in TargetPlayerId)
                {
                    message.VarToPlayerId.Add(playerid);
                };

                foreach (var msg in list)
                {
                    message.VarMessage.Add(ByteString.CopyFrom(msg.ToArray()));
                };

                client.WritePacket((int)PROTOCOL.IdPktAudioMessageReq, message.ToByteArray(), message.ToByteArray().Length);

            }

        }

        static public void SendStopStream()
        {

            if (client == null || client.socket == null || client.socket.Connected == false)
                return;
            {
                STOP_STREAM_REQ message = new STOP_STREAM_REQ();
                message.VarToPlayerId = 0;
              

                client.WritePacket((int)PROTOCOL.IdPktStopStreamReq, message.ToByteArray(), message.ToByteArray().Length);
            }
        }

        static public void SendLogin(string uid,string token)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            if (token == null  || User.LoginSuccess == true)
                return;

            var machineid = DependencyService.Get<MethodExt>().MachineId();
            string Model = DeviceInfo.Model;

            var data = new LOGIN_REQ
            {
                VarUid = uid,
                VarToken = token,
                VarCamName = Model,
                VarMachineId = machineid,
                VarIp = client.IPCheck()
            };
            using (MemoryStream stream = new MemoryStream())
            {
                data.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktLoginReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendVerifyPurchase(string packageid,string purchaseid, string token)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            if (token == null || User.LoginSuccess == false)
                return;
         
            var data = new VERIFY_PURCHASE_REQ
            {
                VarToken = token,
                VarPackageName = packageid,
                VarPurchaseId = purchaseid
            };
            using (MemoryStream stream = new MemoryStream())
            {
                data.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktVerifyPurchaseReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRoomList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var data = new ROOM_LIST_REQ
            {

            };
            using (MemoryStream stream = new MemoryStream())
            {
                data.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomListReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendMakePray(string Content)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            PRAY_MESSAGE_REG_REQ person = new PRAY_MESSAGE_REG_REQ
            {
                VarMessage = Content,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktPrayMessageRegReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendPrayList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var data = new PRAY_MESSAGE_REQ
            {

            };
            using (MemoryStream stream = new MemoryStream())
            {
                data.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktPrayMessageReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendQNS(string msg)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            QNA_REQ message = new QNA_REQ
            {
                VarMessage = msg,

            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktQnaReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRegCamera(string camName, string machineid)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            REG_CAMERA_REQ message = new REG_CAMERA_REQ
            {
                VarCamName = camName,
                VarMachineId = machineid

            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRegCameraReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendDelCamera(string machineid)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            DEL_CAMERA_REQ message = new DEL_CAMERA_REQ
            {
                VarMachineId = machineid

            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktDelCameraReq, stream.ToArray(), stream.ToArray().Length);
            }
        }


        static public void SendWakeUpCamera(long playerId,string machineid)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            CAMERA_WAKE_UP_REQ message = new CAMERA_WAKE_UP_REQ
            {
               VarType = 0,
               VarRoomNumber = 0,
               VarToPlayerId = playerId,
               VarMachineId = machineid,

            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCameraWakeUpReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendReqCameraList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;


            CAMERA_LIST_REQ message = new CAMERA_LIST_REQ
            {
               

            };
            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCameraListReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendMakeRoom(string Name)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            CREATE_ROOM_REQ person = new CREATE_ROOM_REQ
            {
                VarName = Name,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCreateRoomReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendEnterRoom(int id)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            ENTER_ROOM_REQ person = new ENTER_ROOM_REQ
            {
                VarRoomNumber = id,
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktEnterRoomReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendNOTICE()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            NOTICE_REQ person = new NOTICE_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktNoticeReq, stream.ToArray(), stream.ToArray().Length);
            }
        }
        static public void SendMailList()
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            MAIL_LIST_REQ person = new MAIL_LIST_REQ
            {
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktMailListReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendLeaveRoom(int id)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            LEAVE_ROOM_REQ person = new LEAVE_ROOM_REQ
            {
                VarId = User.CacheData.Id,
                VarRoomNumber = id
            };
            using (MemoryStream stream = new MemoryStream())
            {
                person.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktLeaveRoomReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendRoomMessage(string msg)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            // var bytearray = System.Text.Encoding.GetEncoding(949).GetBytes(msg);

            // var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            ROOM_PASS_THROUGH_REQ message = new ROOM_PASS_THROUGH_REQ
            {
                VarMessage = msg,
                VarMessageInt = 0,
                VarRoomNumber = User.CurrentChatViewNumber,
                VarTime = DateTime.Now.ToString("MM/dd/yyyy H:mm")
            };

            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktRoomPassThroughReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void Recoonect(string machineId,string machineName,string token,Int64 Index,string email,string name)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            // var bytearray = System.Text.Encoding.GetEncoding(949).GetBytes(msg);

            // var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            RECONNECT_REQ message = new RECONNECT_REQ
            {
                VarMachineId = machineId,
                VarMachineName = machineName,
                VarEmail = email,
                VarIndex = Index,
                VarToken = token,
                VarName = name
            };

            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktReconnectReq, stream.ToArray(), stream.ToArray().Length);
            }
        }

        static public void SendCameraControl(string machin_id, Int64 player_id,CameraControlType type)
        {
            if (client == null || client.socket == null || client.socket.Connected == false)
                return;

            // var bytearray = System.Text.Encoding.GetEncoding(949).GetBytes(msg);

            // var bytearray = System.Text.Encoding.UTF8.GetBytes(msg);

            CAMERA_CONTROL_REQ message = new CAMERA_CONTROL_REQ
            {
                VarType = type,
                VarPlayerId = player_id,
                VarMachineId = machin_id,
            };

            using (MemoryStream stream = new MemoryStream())
            {
                message.WriteTo(stream);

                client.WritePacket((int)PROTOCOL.IdPktCameraControlReq, stream.ToArray(), stream.ToArray().Length);
            }
        }
    }
}

