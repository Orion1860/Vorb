///
/// VorbPreDecl.h
/// Vorb Engine
///
/// Created by Cristian Zaloj on 2 Jan 2015
/// Copyright 2014 Regrowth Studios
/// All Rights Reserved
///
/// Summary:
/// Pre-declaration helpers
///

#pragma once

#ifndef VorbPreDecl_inl__
#define VorbPreDecl_inl__

// Create empty namespaces for aliasing purposes
namespace vorb {
    namespace core {
    }
    namespace graphics {
    }
    namespace voxel {
    }
    namespace ui {
    }
    namespace io {
    }
    namespace sound {
    }
    namespace ecs {
    }
    namespace net {
    }
}

// Namespace aliases
namespace vcore = vorb::core; ///< Namespace alias vorb::core
namespace vg = vorb::graphics; ///< Namespace alias vorb::graphics
namespace vvox = vorb::voxel; ///< Namespace alias vorb::voxel
namespace vui = vorb::ui; ///< Namespace alias vorb::ui
namespace vio = vorb::io; ///< Namespace alias vorb::io
namespace vsound = vorb::sound; ///< Namespace alias vorb::sound
namespace vecs = vorb::ecs; ///< Namespace alias vorb::ecs
namespace vnet = vorb::net; ///< Namespace alias vorb::net

#define DECL_VORB(CONTAINER, TYPE) namespace vorb { CONTAINER TYPE; }
#define DECL_VCORE(CONTAINER, TYPE) namespace vorb { namespace core { CONTAINER TYPE; }  }
#define DECL_VG(CONTAINER, TYPE) namespace vorb { namespace graphics { CONTAINER TYPE; }  }
#define DECL_VVOX(CONTAINER, TYPE) namespace vorb { namespace voxel { CONTAINER TYPE; }  }
#define DECL_VUI(CONTAINER, TYPE) namespace vorb { namespace ui { CONTAINER TYPE; }  }
#define DECL_VIO(CONTAINER, TYPE) namespace vorb { namespace io { CONTAINER TYPE; }  }
#define DECL_VSOUND(CONTAINER, TYPE) namespace vorb { namespace sound { CONTAINER TYPE; }  }
#define DECL_VECS(CONTAINER, TYPE) namespace vorb { namespace ecs { CONTAINER TYPE; }  }
#define DECL_VNET(CONTAINER, TYPE) namespace vorb { namespace net { CONTAINER TYPE; }  }

#endif // VorbPreDecl_inl__
