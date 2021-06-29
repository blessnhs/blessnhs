using Antioch.View.Chat.CustomCells;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace Antioch
{
    class MyDataTemplateSelector : DataTemplateSelector
    {
        public MyDataTemplateSelector()
        {
            // Retain instances!
            this.incomingDataTemplate = new DataTemplate(typeof(IncomingViewCell));
            this.outgoingDataTemplate = new DataTemplate(typeof(OutgoingViewCell));
            this.sysinfoDataTemplate = new DataTemplate(typeof(SysInfoCell));
        }

        protected override DataTemplate OnSelectTemplate(object item, BindableObject container)
        {
            var messageVm = item as Message;
            if (messageVm == null)
                return null;

            switch(messageVm.MessageType)
            {
                case Message.type.Incoming:
                    return incomingDataTemplate;
                    break;
                case Message.type.Outgoing:
                    return outgoingDataTemplate;
                    break;
                case Message.type.Info:
                    return sysinfoDataTemplate;
                    break;
            }


            return null;
           
        }

        private readonly DataTemplate incomingDataTemplate;
        private readonly DataTemplate outgoingDataTemplate;
        private readonly DataTemplate sysinfoDataTemplate;
    }
}
