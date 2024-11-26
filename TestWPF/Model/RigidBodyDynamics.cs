using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using OCCTK.OCC.gp;

namespace TestWPF.Model;

[Serializable]
public class RigidBodyDynamics : ISerializable
{
    RigidBodyDynamics()
    {
        Base = new();
    }

    Trsf Base { get; set; }

    public void GetObjectData(SerializationInfo info, StreamingContext context) { }
}
