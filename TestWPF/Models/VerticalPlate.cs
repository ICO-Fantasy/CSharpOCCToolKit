//using System.Collections.Generic;

//using OCCTK.Laser;
//using OCCTK.OCC.AIS;

//namespace TestWPF.Models
//{
//    public class VerticalPlate
//    {
//        private double _location;
//        public double Location
//        {
//            get
//            { return _location; }
//            set
//            {
//                _slices = WMakeSimpleClamp.TestMakeVertical(Workpiece,
//                                                                    BasePlate,
//                                                                    Direction,
//                                                                    value,
//                                                                    Clearances,
//                                                                    MinSupportingLen,
//                                                                    CuttingDistance);
//                _location = value;
//            }
//        }
//        private AShape Workpiece { get; }
//        private BasePlate BasePlate { get; }
//        public VerticalPlateDirection Direction { get; set; }
//        public double maxVerticalLength { get; set; }
//        public double Clearances { get; set; }
//        public double MinSupportingLen { get; set; }
//        public double CuttingDistance { get; set; }
//        private List<Piece> _slices { get; set; } = new List<Piece>();
//        public List<Piece> Slices { get { return _slices; } }

//        public VerticalPlate(AShape theWorkpiece, BasePlate theBasePlate, VerticalPlateDirection theDirection, double theValue, double clearancesParameter, double minSupportingLenParameter, double cuttingDistanceParameter)
//        {
//            Workpiece = theWorkpiece;
//            BasePlate = theBasePlate;
//            Direction = theDirection;
//            Clearances = clearancesParameter;
//            MinSupportingLen = minSupportingLenParameter;
//            CuttingDistance = cuttingDistanceParameter;
//            Location = theValue;
//        }
//        public override string ToString()
//        {
//            return Location.ToString("F1");
//        }
//    }
//}
