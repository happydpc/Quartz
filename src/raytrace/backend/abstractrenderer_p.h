/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytrace_global_p.h>
#include <Qt3DRaytrace/qimagedata.h>
#include <Qt3DRaytrace/qrenderimage.h>

#include <QVector>
#include <Qt3DCore/QAspectJob>

class QObject;
class QSurface;

namespace Qt3DCore {
class QAbstractFrameAdvanceService;
} // Qt3DCore

namespace Qt3DRaytrace {
namespace Raytrace {

class BackendNode;
class Entity;
class RenderSettings;

struct NodeManagers;

class AbstractRenderer
{
public:
    enum DirtyFlag {
        EntityDirty    = 1 << 0,
        TransformDirty = 1 << 1,
        GeometryDirty  = 1 << 2,
        TextureDirty   = 1 << 3,
        MaterialDirty  = 1 << 4,
        LightDirty     = 1 << 5,
        CameraDirty    = 1 << 6,
        NoneDirty      = 0,
        AllDirty       = 0xffffff,
    };
    Q_DECLARE_FLAGS(DirtySet, DirtyFlag)

    virtual ~AbstractRenderer() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    virtual void markDirty(DirtySet changes, BackendNode *node) = 0;

    virtual QSurface *surface() const = 0;
    virtual Entity *sceneRoot() const = 0;
    virtual RenderSettings *settings() const = 0;
    virtual QRenderStatistics statistics() const = 0;

    virtual void setSurface(QObject *surfaceObject) = 0;
    virtual void setSceneRoot(Entity *rootEntity) = 0;
    virtual void setSettings(RenderSettings *settings) = 0;
    virtual void setNodeManagers(NodeManagers *nodeManagers) = 0;

    virtual QImageData grabImage(QRenderImage type) = 0;

    virtual Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const = 0;

    virtual QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) = 0;
};

} // Raytrace
} // Qt3DRaytrace
