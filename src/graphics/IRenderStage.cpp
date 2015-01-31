#include "stdafx.h"
#include "graphics/IRenderStage.h"


vg::IRenderStage::IRenderStage(const Camera* camera /* = nullptr */) :
    _camera(camera),
    _isVisible(true) {
    // Empty
}


vg::IRenderStage::~IRenderStage() {
    // Empty
}