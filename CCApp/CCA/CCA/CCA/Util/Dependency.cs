using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace DependencyHelper
{
    public interface FlatformPath
    {
        string GetPath();

    }

    public interface MethodExt
    {
        string MachineId();
        string MachineName();

        void Notification(string message);

        void Logout();

        void Purchase(string purchaseid);
    }
}