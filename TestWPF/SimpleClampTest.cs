using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using OCCTK.IO;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepAlgoAPI;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;
using TestWPF.Laser.SimpleClamp;
using WinRT;

namespace TestWPF;

public partial class SimpleClamp : Window
{
    private void OCCFunctionTest()
    {
        #region Explorer
        //Explorer faceExp = new(InputWorkpiece.Shape, ShapeEnum.EDGE);
        //int faceNum = 0;
        //while (faceExp.More())
        //{
        //    faceNum += 1;
        //    var currentEdge = faceExp.Current().AsEdge();
        //    Explorer PntExp = new(currentEdge, ShapeEnum.VERTEX);
        //    int edgeNum = 0;
        //    for (var endPoint = PntExp.Current().AsVertex(); PntExp.More(); PntExp.Next())
        //    {
        //        edgeNum += 1;
        //    }
        //    Debug.WriteLine($"Face: {faceNum},EndPoint:{edgeNum}");
        //    faceExp.Next();
        //}
        #endregion

        #region make Wire
        //MakeWire wireMaker = new();
        //wireMaker.Add(new MakeEdge(new Pnt(0, 0, 0), new Pnt(1, 0, 0)));
        //wireMaker.Add(new MakeEdge(new Pnt(1, 0, 0), new Pnt(2, 0, 0)));
        //Debug.WriteLine($"Wire is done: {wireMaker.Error()}");
        #endregion

        #region inputWorkpiece
        Workpiece testWorkpiece;
        testWorkpiece = new Workpiece(new BrepExchange("mods\\new_test01.brep"));
        Display(testWorkpiece, false);
        //this.AISContext.SetColor(testWorkpiece.AIS, new OCCTK.Extension.Color(255, 0, 0), true);
        //testWorkpiece = new Workpiece(new BrepExchange("mods\\new_test01.brep"));
        //testWorkpiece.Dispose();
        //Display(testWorkpiece.AIS, true);

        #endregion

        #region basePlate
        BasePlate basePlate = new(testWorkpiece);
        Display(basePlate, true);

        #endregion
        #region IntersectionEdge

        PlatePose testPose = new(new(10, 10, basePlate.Z), new(1, 0, 0));
        Display(testPose, true);
        Splitter splitter = new Splitter();
        splitter.SetArguments(testWorkpiece);
        splitter.SetTools(testPose);
        splitter.Build();
        foreach (var edge in splitter.SectionEdges())
        {
            Display(new AShape(edge), false);
        }
        this.Canvas.Update();
        #endregion
    }
}
