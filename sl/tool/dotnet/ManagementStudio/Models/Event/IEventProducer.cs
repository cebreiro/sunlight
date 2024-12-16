using Sunlight.ManagementStudio.Models.Event.Args;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sunlight.ManagementStudio.Models.Event
{
    public interface IEventProducer
    {
        void Produce(DisconnectionEventArgs args);
    }
}
