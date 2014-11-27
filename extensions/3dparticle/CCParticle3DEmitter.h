/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __CC_PARTICLE_3D_EMITTER_H__
#define __CC_PARTICLE_3D_EMITTER_H__

#include "base/CCRef.h"
#include "math/CCMath.h"
#include "3dparticle/ParticleUniverse/CCParticle3DDynamicAttribute.h"
#include <vector>

NS_CC_BEGIN
struct Particle3D;
class ParticleSystem3D;
/**
 * 3d particle emitter
 */
class Particle3DEmitter : public Ref
{
    friend class ParticleSystem3D;
public:

    // Default values
    static const bool DEFAULT_ENABLED;
    static const Vec3 DEFAULT_POSITION;
    static const bool DEFAULT_KEEP_LOCAL;
    static const Vec3 DEFAULT_DIRECTION;
    static const Quaternion DEFAULT_ORIENTATION;
    static const Quaternion DEFAULT_ORIENTATION_RANGE_START;
    static const Quaternion DEFAULT_ORIENTATION_RANGE_END;
    //static const Particle::ParticleType DEFAULT_EMITS;
    static const unsigned short DEFAULT_START_TEXTURE_COORDS;
    static const unsigned short DEFAULT_END_TEXTURE_COORDS;
    static const unsigned short DEFAULT_TEXTURE_COORDS;
    static const Vec4 DEFAULT_START_COLOUR_RANGE;
    static const Vec4 DEFAULT_END_COLOUR_RANGE;
    static const Vec4 DEFAULT_COLOUR;
    static const bool DEFAULT_AUTO_DIRECTION;
    static const bool DEFAULT_FORCE_EMISSION;
    static const float DEFAULT_EMISSION_RATE;
    static const float DEFAULT_TIME_TO_LIVE;
    static const float DEFAULT_MASS;
    static const float DEFAULT_VELOCITY;
    static const float DEFAULT_DURATION;
    static const float DEFAULT_REPEAT_DELAY;
    static const float DEFAULT_ANGLE;
    static const float DEFAULT_DIMENSIONS;
    static const float DEFAULT_WIDTH;
    static const float DEFAULT_HEIGHT;
    static const float DEFAULT_DEPTH;

    Particle3DEmitter();
    virtual ~Particle3DEmitter();
    
    virtual void notifyStart();
    virtual void notifyStop();
    virtual void notifyPause();
    virtual void notifyResume();
    virtual void notifyRescaled(const Vec3& scale);
    virtual void prepare();
    virtual void unPrepare();
    virtual void preUpdateEmitter(float deltaTime);
    virtual void updateEmitter(Particle3D *particle, float deltaTime);
    virtual void postUpdateEmitter(float deltaTime);
    virtual unsigned short calculateRequestedParticles(float timeElapsed);
    
    virtual void emit(int count);

        /** Calculate the derived position of the affector.
    @remarks
        Note, that in script, the position is set as localspace, while if the affector is
        emitted, its position is automatically transformed. This function always returns 
        the derived position.
    */
    const Vec3& getDerivedPosition();
    
    /** Enables or disables the emitter.
    */
    void setEnabled (bool enabled);

    bool isEnabled(void) const;

        /** Todo
    */
    inline const std::string& getEmitterType(void) const {return _emitterType;};
    void setEmitterType(const std::string& emitterType) {_emitterType = emitterType;};

    /** Todo
    */
    inline const std::string& getName(void) const {return _name;};
    void setName(const std::string& name) {_name = name;};

    /** Todo
    */
    inline DynamicAttribute* getDynAngle(void) const {return _dynAngle;};
    void setDynAngle(DynamicAttribute* dynAngle);

    /** Todo
    */
    inline DynamicAttribute* getDynEmissionRate(void) const {return _dynEmissionRate;};
    void setDynEmissionRate(DynamicAttribute* dynEmissionRate);

    /** Todo
    */
    inline DynamicAttribute* getDynTotalTimeToLive(void) const {return _dynTotalTimeToLive;};
    void setDynTotalTimeToLive(DynamicAttribute* dynTotalTimeToLive);

    /** Todo
    */
    inline DynamicAttribute* getDynParticleMass(void) const {return _dynParticleMass;};
    void setDynParticleMass(DynamicAttribute* dynParticleMass);

    /** Todo
    */
    inline DynamicAttribute* getDynVelocity(void) const {return _dynVelocity;};
    void setDynVelocity(DynamicAttribute* dynVelocity);

    /** Todo
    */
    inline DynamicAttribute* getDynDuration(void) const {return _dynDuration;};
    void setDynDuration(DynamicAttribute* dynDuration);
    void setDynDurationSet(bool durationSet);

    /** Todo
    */
    inline DynamicAttribute* getDynRepeatDelay(void) const {return _dynRepeatDelay;};
    void setDynRepeatDelay(DynamicAttribute* dynRepeatDelay);
    void setDynRepeatDelaySet(bool repeatDelaySet);

    /** Todo
    */
    inline DynamicAttribute* getDynParticleAllDimensions(void) const {return _dynParticleAllDimensions;};
    void setDynParticleAllDimensions(DynamicAttribute* dynParticleAllDimensions);
    void setDynParticleAllDimensionsSet(bool particleAllDimensionsSet);

    /** Todo
    */
    inline DynamicAttribute* getDynParticleWidth(void) const {return _dynParticleWidth;};
    void setDynParticleWidth(DynamicAttribute* dynParticleWidth);
    void setDynParticleWidthSet(bool particleWidthSet);

    /** Todo
    */
    inline DynamicAttribute* getDynParticleHeight(void) const {return _dynParticleHeight;};
    void setDynParticleHeight(DynamicAttribute* dynParticleHeight);
    void setDynParticleHeightSet(bool particleHeightSet);

    /** Todo
    */
    inline DynamicAttribute* getDynParticleDepth(void) const {return _dynParticleDepth;};
    void setDynParticleDepth(DynamicAttribute* dynParticleDepth);
    void setDynParticleDepthSet(bool particleDepthSet);

    ///** Todo
    //*/
    //inline ParticleType getEmitsType(void) const {return mEmitsType;};
    //void setEmitsType(ParticleType emitsType) {mEmitsType = emitsType;};

    /** Todo
    */
    inline const std::string& getEmitsName(void) const {return _emitsName;};
    void setEmitsName(const std::string& emitsName);

        /** Returns the base direction of the particle that is going to be emitted.
    */
    const Vec3& getParticleDirection(void);

    /** Returns the originally set particle direction. This value is not affected by affectors, angle, etc.
    */
    const Vec3& getOriginalParticleDirection(void) const;

    /** Returns the base orientation of the particle that is going to be emitted.
    */
    const Quaternion& getParticleOrientation(void) const;

    /** Set the orientation of the particle.
    */
    void setParticleOrientation(const Quaternion& orientation);

    /** Returns the start orientation of the particle that is going to be emitted.
    @remarks
        The orientation is generated random between mParticleOrientationRangeStart and mParticleOrientationRangeEnd.
    */
    const Quaternion& getParticleOrientationRangeStart(void) const;

    /** Set start orientation of the particle that is going to be emitted.
    @remarks
        The orientation is generated random between mParticleOrientationRangeStart and mParticleOrientationRangeEnd.
    */
    void setParticleOrientationRangeStart(const Quaternion& orientationRangeStart);

    /** Returns the end orientation of the particle that is going to be emitted.
    @remarks
        The orientation is generated random between mParticleOrientationRangeStart and mParticleOrientationRangeEnd.
    */
    const Quaternion& getParticleOrientationRangeEnd(void) const;

    /** Set end orientation of the particle that is going to be emitted.
    @remarks
        The orientation is generated random between mParticleOrientationRangeStart and mParticleOrientationRangeEnd.
    */
    void setParticleOrientationRangeEnd(const Quaternion& orientationRangeEnd);
            
    /** Sets the direction of the particle that the emitter is emitting.
    @remarks
        Don't confuse this with the emitters own direction.
    @param dir The base direction of emitted particles.
    */
    void setParticleDirection(const Vec3& direction);

    /** Todo
    */
    bool isAutoDirection(void) const;

    /** Todo
    */
    void setAutoDirection(bool autoDirection);

    /** Todo
    */
    bool isForceEmission(void) const;

    /** Todo
    */
    void setForceEmission(bool forceEmission);

        /** Get the colour of a particle that will be emitted.
    */
    const Vec4& getParticleColor(void) const;

    /** Set the colour of an emitted particle.
    */
    void setParticleColor(const Vec4& particleColour);

    /** Get the colour range start of an emitted particle.
    */
    const Vec4& getParticleColorRangeStart(void) const;

    /** Set the colour range start of an emitted particle. This is the lower value used to generate a random colour.
    */
    void setParticleColorRangeStart(const Vec4& particleColourRangeStart);

    /** Get the colour range end of an emitted particle.
    */
    const Vec4& getParticleColorRangeEnd(void) const;

    /** Set the colour range end of an emitted particle. This is the upper value used to generate a random colour.
    */
    void setParticleColorRangeEnd(const Vec4& particleColourRangeEnd);

    /** Get the texture coords of an emitted particle.
    */
    const unsigned short& getParticleTextureCoords(void) const;

    /** Set the texture coords of an emitted particle.
    */
    void setParticleTextureCoords(const unsigned short& particleTextureCoords);

    /** Get the texture coords range start  of an emitted particle.
    */
    const unsigned short& getParticleTextureCoordsRangeStart(void) const;

    /** Set the texture coords range start of an emitted particle. This is the lower value used to set a random texture coords.
    */
    void setParticleTextureCoordsRangeStart(const unsigned short& particleTextureCoordsRangeStart);
            
    /** Get the texture coords range end of an emitted particle.
    */
    const unsigned short& getParticleTextureCoordsRangeEnd(void) const;

    /** Set the texture coords range end of an emitted particle. This is the upper value used to set a random texture coords.
    */
    void setParticleTextureCoordsRangeEnd(const unsigned short& particleTextureCoordsRangeEnd);

    /** Todo
    */
    bool isKeepLocal(void) const;

    /** If this attribute is set to 'true', the particles are emitted relative to the emitter
    */
    void setKeepLocal(bool keepLocal);

    /** Transforms the particle position in a local position relative to the emitter
    */
    bool makeParticleLocal(Particle3D* particle);

protected:

    /** Todo
    */
    virtual void initParticlePosition(Particle3D* particle);

    /**  Internal method for generating the particle direction.
    */
    virtual void initParticleDirection(Particle3D* particle);

    /**  Internal method for generating the particle orientation.
    */
    virtual void initParticleOrientation(Particle3D* particle);

	virtual void initParticleVelocity(Particle3D* particle);

	virtual void initParticleMass(Particle3D* particle);

	virtual void initParticleColor(Particle3D* particle);

	virtual void initParticleTextureCoords(Particle3D* particle);

	virtual float initParticleTimeToLive();

	virtual void initParticleDimensions(Particle3D* particle);

	virtual void initParticleForEmission(Particle3D* particle);

    /** Initialise some attributes that are time-based.
    */
    inline void initTimeBased(void);

        /**  Internal method for generating the angle.
    */
    void generateAngle(float angle);

    
protected:

    ParticleSystem3D* _particleSystem;

    Vec3 _latestPosition;

    Vec3 _derivedPosition;
    /** Although the scale is on a Particle System level, the emitter can also be scaled.
    */
    Vec3 _emitterScale;

    // Type of the emitter
    std::string _emitterType;

    // Name of the emitter (optional)
    std::string _name;

    /** Direction (and speed) of the emitted particle
    @remarks
        Don't confuse the particle direction with the direction of the emitter itself. 
        The particleDirection is the direction of an emitted particle.
    */
    Vec3 _particleDirection;
            
    /** The original direction of the emitted particle
    @remarks
        Don't confuse this with the particles' originalDirection.
    */
    Vec3 _originalParticleDirection;

    /** Orientation of the particle as soon as it is emitted.
    @remarks
        This is only visible if a renderer is used that renders 3D particles.
    */
    Quaternion _particleOrientation;

    /** If set, the range generates a random orientation between start and end.
    */
    Quaternion _particleOrientationRangeStart;
    Quaternion _particleOrientationRangeEnd;
    bool _particleOrientationRangeSet;

    // Angle around direction which particles may be emitted
    DynamicAttribute* _dynAngle;

    // Rate of particle emission.
    DynamicAttribute* _dynEmissionRate;

    //// Identifies the type of particle this emitter emits (default is visual particle).
    //ParticleType mEmitsType;

    // Identifies the name of particle this emitter emits (default is visual particle, so there is no name)
    std::string _emitsName;

    /** Dynamic attribute used to generate the total time to live.
    */
    DynamicAttribute* _dynTotalTimeToLive;

    /** Dynamic attribute used to generate the mass of a particle.
    */
    DynamicAttribute* _dynParticleMass;

    /** Dynamic attribute used to generate the velocity of a particle.
    */
    DynamicAttribute* _dynVelocity;

    /** Dynamic attribute used to define the duration of the emitter.
    */
    DynamicAttribute* _dynDuration;

    /** Dynamic attribute used to define the repeat/delay of the emitter.
    */
    DynamicAttribute* _dynRepeatDelay;

    /** Dynamic attribute used to define the (own) dimensions of a particle.
    @remarks
        In the Particle Technique it is possible to set the default dimensions for all particles, but it
        is also possible to set the dimensions per particle. Note, that this only applies to visual particles,
        of course. This attribute is used to x, y and z dimensions with the value.
    */
    DynamicAttribute* _dynParticleAllDimensions;
    bool _dynParticleAllDimensionsSet;

    /** Dynamic attribute used to define the (own) width of a particle.
    @remarks
        In the Particle Technique it is possible to set the default width for all particles, but it
        is also possible to set the width per particle. Note, that this only applies to visual particles,
        of course.
    */
    DynamicAttribute* _dynParticleWidth;
    bool _dynParticleWidthSet;

    /** Dynamic attribute used to define the (own) height of a particle.
    @remarks
        In the Particle Technique it is possible to set the default height for all particles, but it
        is also possible to set the height per particle. Note, that this only applies to visual particles,
        of course.
    */
    DynamicAttribute* _dynParticleHeight;
    bool _dynParticleHeightSet;

    /** Dynamic attribute used to define the (own) depth of a particle.
    @remarks
        In the Particle Technique it is possible to set the default depth for all particles, but it
        is also possible to set the depth per particle. Note, that this only applies to visual particles,
        of course.
    */
    DynamicAttribute* _dynParticleDepth;
    bool _dynParticleDepthSet;

    /** Notional up vector, just used to speed up generation of variant directions.
    */
    Vec3 _upVector;

    /** Particles that are left to be emitted from the previous time.
    */
    float _remainder;

    ///** Helper factory
    //*/
    //DynamicAttributeFactory mDynamicAttributeFactory;

    /** Helper class
    */
    DynamicAttributeHelper _dynamicAttributeHelper;

    /** Duration remainder.
    */
    float _durationRemain;

    /** Indication to determine whether duration is set.
    */
    bool _dynDurationSet;

    /** Repeat/delay remainder.
    */
    float _repeatDelayRemain;

    /** Indication to determine whether repeat/delay is set.
    */
    bool _dynRepeatDelaySet;

    /** Auto direction means that the direction of a particle is not determined by the 
        direction vector that has been set, but the direction vector is generated based on
        the shape of the emitter. In most cases this means that the particle direction is
        perpendicular on the shape of the emitter.
    @remarks
        This attribute has only a meaning for certain emitters.
    */
    bool _autoDirection;

    /** In normal cases, emission is regulated by time. This prevents that too much particles are
        emitted at once, which causes a drop in the framerate. In some cases you just want to emit
        all particles immediately without any delay. Setting the 'mForceEmission' to true enables this.
        In that case, the emission rate defines the number of particles that are emitted immediately.
        If the emission rate is 100, the emitter emits 100 particles at once and then disables.
    */
    bool _forceEmission;

    /** Original value, used to set it back.
    */
    bool _originalForceEmission;

    /** If mForceEmission has been set to true and if all particles are emitted, the mForceEmissionExecuted
        is also set to true, preventing any further 'forced emission'.
    */
    bool _forceEmissionExecuted;

    /** Original value, used to set it back.
    */
    bool _originalForceEmissionExecuted;

    /** Colour that is assigned to an emitted particle.
    */
    Vec4 _particleColor;

    /** Used to randomize the colour of an emitted particle.
    */
    Vec4 _particleColorRangeStart;

    /** Used to randomize the colour of an emitted particle.
    */
    Vec4 _particleColorRangeEnd;

    /** Used to determine whether the colour range has been set.
    */
    bool _particleColorRangeSet;

    /** Determines whether particle positions should be kept local in relation to the emitter.
    */
    bool _keepLocal;

    /** Texture coords that is assigned to an emitted particle (only works if the renderer uses it)
    */
    unsigned short _particleTextureCoords;

    /** Used to randomize the texture coords of an emitted particle.
    */
    unsigned short _particleTextureCoordsRangeStart;
                
    /** Used to randomize the texture coords of an emitted particle.
    */
    unsigned short _particleTextureCoordsRangeEnd;

    /** Used to determine whether the texture coords range has been set.
    */
    bool _particleTextureCoordsRangeSet;

    bool _isEnabled;
};

NS_CC_END

#endif
