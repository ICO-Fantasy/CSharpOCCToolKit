#pragma once
#include<Prs3d_DatumParts.hxx>
namespace OCCTK {
namespace OCC {
namespace AIS {

public enum class DatumParts {
	XAxis = Prs3d_DatumParts::Prs3d_DatumParts_XAxis,
	YAxis = Prs3d_DatumParts::Prs3d_DatumParts_YAxis,
	ZAxis = Prs3d_DatumParts::Prs3d_DatumParts_ZAxis,
};

}
}
}