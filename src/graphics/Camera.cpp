#include "stdafx.h"
#include "graphics/Camera.h"

//////////////////////////////////////////////////////////////////////////
//                          General 3D Camera                           //
//////////////////////////////////////////////////////////////////////////

#define FXX typename vg::Camera3D<T>::fXX
#define FXXV2 typename vg::Camera3D<T>::fXXv2
#define FXXV3 typename vg::Camera3D<T>::fXXv3
#define FXXV4 typename vg::Camera3D<T>::fXXv4
#define FXXQ typename vg::Camera3D<T>::fXXq
#define FXXM4 typename vg::Camera3D<T>::fXXm4

template <class T>
const FXXV3 vg::Camera3D<T>::ORIG_DIRECTION = FXXV3(1.0, 0.0, 0.0);
template <class T>
const FXXV3 vg::Camera3D<T>::ORIG_RIGHT = FXXV3(0.0, 0.0, 1.0);
template <class T>
const FXXV3 vg::Camera3D<T>::ORIG_UP = FXXV3(0.0, 1.0, 0.0);

template <class T>
vg::Camera3D<T>::Camera3D() {
    static_assert(std::numeric_limits<T>::is_iec559, "Camera3D only accepts floating-point template types.");
}

template <class T>
void vg::Camera3D<T>::init(FXX aspectRatio, FXX fieldOfView) {
    m_aspectRatio = aspectRatio;
    m_fieldOfView = fieldOfView;
}

template <class T>
void vg::Camera3D<T>::update(FXX deltaTime) {
    bool updateFrustum = false;
    if (m_viewChanged) {
        updateView();
        m_viewChanged = false;
        updateFrustum = true;
    }
    if (m_projectionChanged) {
        updateProjection();
        m_projectionChanged = false;
        updateFrustum = true;
    }

    if (updateFrustum) {
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_frustum.updateFromWVP(m_viewProjectionMatrix);
    }
}

template <class T>
void vg::Camera3D<T>::updateView() {
    m_viewMatrix = vmath::lookAt(FXXV3(0.0), getDirection(), getUp());
}

template <class T>
void vg::Camera3D<T>::updateProjection() {
    m_frustum.setCamInternals(m_fieldOfView, m_aspectRatio, m_zNear, m_zFar);
    m_projectionMatrix = vmath::perspective(m_fieldOfView, m_aspectRatio, m_zNear, m_zFar);
}

template <class T>
void vg::Camera3D<T>::offsetPosition(const FXXV3& offset) {
    m_position += offset;
    m_viewChanged = true;
}

template <class T>
void vg::Camera3D<T>::applyRotation(const FXXQ& rot) {
    m_directionQuat = rot * m_directionQuat;

    m_viewChanged = true;
}

template <class T>
void vg::Camera3D<T>::applyRotation(FXX angle, const FXXV3& axis) {
    applyRotation(vmath::angleAxis(angle, axis));
}

template <class T>
void vg::Camera3D<T>::applyRoll(FXX angle) {
    applyRotation(vmath::angleAxis(angle, getDirection()));
}

template <class T>
void vg::Camera3D<T>::applyYaw(FXX angle) {
    applyRotation(vmath::angleAxis(angle, getUp()));
}

template <class T>
void vg::Camera3D<T>::applyPitch(FXX angle) {
    applyRotation(vmath::angleAxis(angle, getRight()));
}

template <class T>
void vg::Camera3D<T>::rotateFromMouse(FXX dx, FXX dy, FXX speed) {
    FXXQ upQuat = vmath::angleAxis(dy * speed, getRight());
    FXXQ rightQuat = vmath::angleAxis(dx * speed, getUp());

    applyRotation(upQuat * rightQuat);
}

template <class T>
void vg::Camera3D<T>::rollFromMouse(FXX dx, FXX speed) {
    FXXQ frontQuat = vmath::angleAxis(dx * speed, getDirection());

    applyRotation(frontQuat);
}

template <class T>
FXXV3 vg::Camera3D<T>::worldToScreenPoint(const FXXV3& worldPoint) const {
    // Transform world to clipping coordinates
    FXXV4 clipPoint = m_viewProjectionMatrix * FXXV4(worldPoint, 1.0);
    clipPoint.x /= clipPoint.w;
    clipPoint.y /= clipPoint.w;
    clipPoint.z /= clipPoint.w;
    return FXXV3((clipPoint.x + (FXX)1.0) / (FXX)2.0,
        ((FXX)1.0 - clipPoint.y) / (FXX)2.0,
        clipPoint.z);
}

template <class T>
FXXV3 vg::Camera3D<T>::worldToScreenPointLogZ(const FXXV3& worldPoint, FXX zFar) const {
    // Transform world to clipping coordinates
    FXXV4 clipPoint = m_viewProjectionMatrix * FXXV4(worldPoint, 1.0);
    clipPoint.z = log2(vmath::max((FXX)0.0001, clipPoint.w + (FXX)1.0)) * (FXX)2.0 / log2(zFar + (FXX)1.0) - (FXX)1.0;
    clipPoint.x /= clipPoint.w;
    clipPoint.y /= clipPoint.w;
    return FXXV3((clipPoint.x + (FXX)1.0) / (FXX)2.0,
        ((FXX)1.0 - clipPoint.y) / (FXX)2.0,
        clipPoint.z);
}

template <class T>
FXXV3 vg::Camera3D<T>::getPickRay(const FXXV2& ndcScreenPos) const {
    FXXV4 clipRay(ndcScreenPos.x, ndcScreenPos.y, -1.0, 1.0);
    FXXV4 eyeRay = vmath::inverse(m_projectionMatrix) * clipRay;
    eyeRay = FXXV4(eyeRay.x, eyeRay.y, -1.0, 0.0);
    return vmath::normalize(FXXV3(vmath::inverse(m_viewMatrix) * eyeRay));
}

//////////////////////////////////////////////////////////////////////////
//                         Cinematic 3D Camera                          //
//////////////////////////////////////////////////////////////////////////

// Nothing so far.

//////////////////////////////////////////////////////////////////////////
//                            FPS 3D Camera                             //
//////////////////////////////////////////////////////////////////////////

template <class T>
void vg::FPSCamera3D<T>::update(FXX deltaTime) {
    if (m_wobbleEnabled) {
        updateWobble(deltaTime);
    }

    bool updateFrustum = false;
    if (m_viewChanged) {
        updateView();
        m_viewChanged = false;
        updateFrustum = true;
    }
    if (m_projectionChanged) {
        updateProjection();
        m_projectionChanged = false;
        updateFrustum = true;
    }

    if (updateFrustum) {
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_frustum.updateFromWVP(m_viewProjectionMatrix);
    }
}

template <class T>
void vg::FPSCamera3D<T>::updateWobble(FXX deltaTime) {
    m_wobbleStage += deltaTime;
    if (m_wobbleStage > m_wobblePeriod) {
        FXX deltaStage = m_wobbleStage - m_wobblePeriod;
        m_wobbleStage = ((T)-1.0 * m_wobblePeriod) + deltaStage;
    }

    FXX targetRoll = m_wobbleTween((T)-1.0 * m_wobbleAmplitude, m_wobbleAmplitude, vmath::abs(m_wobbleStage) / m_wobblePeriod);
    FXX currentRoll = vmath::roll(m_directionQuat);
    FXX deltaRoll = targetRoll - currentRoll;

    m_directionQuat = vmath::angleAxis(deltaRoll, getDirection()) * m_directionQuat;
}

template <class T>
void vg::FPSCamera3D<T>::applyRotation(const FXXQ& rot) {
    m_directionQuat = rot * m_directionQuat;

    if (m_lockPitch) {
        T newPitch = vmath::pitch(m_directionQuat);
        if (newPitch > m_pitchLimit) {
            T deltaPitch = newPitch - m_pitchLimit;
            m_directionQuat = vmath::angleAxis(deltaPitch, getRight()) * m_directionQuat;
        } else if (newPitch < (FXX)-1.0 * m_pitchLimit) {
            T deltaPitch = newPitch + m_pitchLimit;
            m_directionQuat = vmath::angleAxis(deltaPitch, getRight()) * m_directionQuat;
        }
    }

    if (m_lockRoll) {
        T newRoll = vmath::roll(m_directionQuat);
        if (newRoll > m_rollLimit) {
            T deltaRoll = newRoll - m_rollLimit;
            m_directionQuat = vmath::angleAxis(deltaRoll, getDirection()) * m_directionQuat;
        } else if (newRoll < (FXX)-1.0 * m_rollLimit) {
            T deltaRoll = newRoll + m_rollLimit;
            m_directionQuat = vmath::angleAxis(deltaRoll, getDirection()) * m_directionQuat;
        }
    }

    m_viewChanged = true;
}

template <class T>
void vg::FPSCamera3D<T>::stabiliseRoll() {
    T currRoll = vmath::roll(m_directionQuat);
    applyRoll((FXX)-1.0 * currRoll);
}