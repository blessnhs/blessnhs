using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using Xamarin.Forms;

namespace OMOK.ViewModels
{
    public class CommunityRoomInfoModel
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int CurrentCount { get; set; }
    };

    class CommunityViewModel
    {
        // Collection의 변화(Add/Delete)를 자동으로 감지하여 UI화면을 자동갱신
        // ObservableCollection은 INotyfyChanged 인터페이스를 구현했다.
        // ListView를 자동으로 업데이트 하기위해 ObservableCollection을 사용.

        private ObservableCollection<CommunityRoomInfoModel> roommodel = new ObservableCollection<CommunityRoomInfoModel>();

        public ObservableCollection<CommunityRoomInfoModel> RoomModel
        {
            get
            {
                return roommodel;
            }

            set
            {
                roommodel = value;
            }
        }

        public CommunityViewModel()
        {
            RoomModel = new ObservableCollection<CommunityRoomInfoModel>();
        }
    }
}