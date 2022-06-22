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

    public interface Toast
    {
        void Show(string message);

        void StartRecord(object view);

        void Notification(string message);

    }
}