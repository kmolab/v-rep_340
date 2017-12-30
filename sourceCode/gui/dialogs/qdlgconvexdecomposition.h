// This file is part of V-REP, the Virtual Robot Experimentation Platform.
// 
// Copyright 2006-2017 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// V-REP is dual-licensed, under the terms of EITHER (at your option):
//   1. V-REP commercial license (contact us for details)
//   2. GNU GPL (see below)
// 
// GNU GPL license:
// -------------------------------------------------------------------
// V-REP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// V-REP IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with V-REP.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.4.0 rev. 1 on April 5th 2017

#ifndef QDLGCONVEXDECOMPOSITION_H
#define QDLGCONVEXDECOMPOSITION_H

#include "vDialog.h"

namespace Ui {
    class CQDlgConvexDecomposition;
}

class CQDlgConvexDecomposition : public VDialog
{
    Q_OBJECT

public:
    explicit CQDlgConvexDecomposition(QWidget *parent = 0);
    ~CQDlgConvexDecomposition();

    void cancelEvent();
    void okEvent();

    void refresh();


    bool individuallyConsiderMultishapeComponents;
    int maxIterations;
    bool useHACD;

    // HACD params:
    bool addExtraDistPoints;
    bool addFacesPoints;
    int nClusters;
    int maxHullVertices;
    float maxConcavity;
    float smallClusterThreshold;
    int maxTrianglesInDecimatedMesh;
    float maxConnectDist;

    // VHACD params:
    int resolution;
    int depth;
    float concavity;
    int planeDownsampling;
    int convexHullDownsampling;
    float alpha;
    float beta;
    float gamma;
    bool pca;
    bool voxelBasedMode;
    int maxNumVerticesPerCH;
    float minVolumePerCH;


private slots:

    void on_qqOkCancel_accepted();
    void on_qqOkCancel_rejected();
    void on_qqExtraDistPoints_clicked();
    void on_qqExtraFacesPoints_clicked();
    void on_qqClusters_editingFinished();
    void on_qqConcavity_editingFinished();
    void on_qqConnectDist_editingFinished();
    void on_qqTargetBlabla_editingFinished();
    void on_qqMaxHullVertices_editingFinished();
    void on_qqSmallClusterThreshold_editingFinished();
    void on_qqIndividualComponents_clicked();
    void on_qqMaxIterations_editingFinished();
    void on_qqPcaEnabled_clicked();
    void on_qqVoxelBased_clicked();
    void on_qqResolution_editingFinished();
    void on_qqDepth_editingFinished();
    void on_qqConcavity_2_editingFinished();
    void on_qqPlaneDownsampling_editingFinished();
    void on_qqConvexHullDownsampling_editingFinished();
    void on_qqAlpha_editingFinished();
    void on_qqBeta_editingFinished();
    void on_qqGamma_editingFinished();
    void on_qqMaxVerticesPerCh_editingFinished();
    void on_qqMinVolumePerCh_editingFinished();
    void on_qqHACD_clicked();
    void on_qqVHACD_clicked();

private:
    Ui::CQDlgConvexDecomposition *ui;
};

#endif // QDLGCONVEXDECOMPOSITION_H
