// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// THIS FILE WAS AUTO-GENERATED BY ./wpiunits/generate_units.py. DO NOT MODIFY

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.*;
import edu.wpi.first.units.*;

@SuppressWarnings({"unchecked", "cast", "checkstyle", "PMD"})
public interface Angle extends Measure<AngleUnit> {
  static  Angle ofRelativeUnits(double magnitude, AngleUnit unit) {
    return new ImmutableAngle(magnitude, unit.toBaseUnits(magnitude), unit);
  }

  static  Angle ofBaseUnits(double baseUnitMagnitude, AngleUnit unit) {
    return new ImmutableAngle(unit.fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, unit);
  }

  @Override
  Angle copy();

  @Override
  default MutAngle mutableCopy() {
    return new MutAngle(magnitude(), baseUnitMagnitude(), unit());
  }

  @Override
  AngleUnit unit();

  @Override
  default AngleUnit baseUnit() { return (AngleUnit) unit().getBaseUnit(); }

  @Override
  default double in(AngleUnit unit) {
    return unit.fromBaseUnits(baseUnitMagnitude());
  }

  @Override
  default Angle unaryMinus() {
    return (Angle) unit().ofBaseUnits(0 - baseUnitMagnitude());
  }

  @Override
  @Deprecated(since = "2025", forRemoval = true)
  @SuppressWarnings({"deprecation", "removal"})
  default Angle negate() {
    return (Angle) unaryMinus();
  }

  @Override
  default Angle plus(Measure<? extends AngleUnit> other) {
    return (Angle) unit().ofBaseUnits(baseUnitMagnitude() + other.baseUnitMagnitude());
  }

  @Override
  default Angle minus(Measure<? extends AngleUnit> other) {
    return (Angle) unit().ofBaseUnits(baseUnitMagnitude() - other.baseUnitMagnitude());
  }

  @Override
  default Angle times(double multiplier) {
    return (Angle) unit().ofBaseUnits(baseUnitMagnitude() * multiplier);
  }

  @Override
  default Angle divide(double divisor) {
    return (Angle) unit().ofBaseUnits(baseUnitMagnitude() / divisor);
  }

  @Override
  default AngularVelocity per(TimeUnit period) {
    return divide(period.of(1));
  }


  @Override
  default Mult<AngleUnit, AccelerationUnit<?>> times(Acceleration<?> multiplier) {
    return (Mult<AngleUnit, AccelerationUnit<?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, AccelerationUnit<?>> divide(Acceleration<?> divisor) {
    return (Per<AngleUnit, AccelerationUnit<?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, AngleUnit> times(Angle multiplier) {
    return (Mult<AngleUnit, AngleUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Dimensionless divide(Angle divisor) {
    return Value.of(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }


  @Override
  default Mult<AngleUnit, AngularAccelerationUnit> times(AngularAcceleration multiplier) {
    return (Mult<AngleUnit, AngularAccelerationUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, AngularAccelerationUnit> divide(AngularAcceleration divisor) {
    return (Per<AngleUnit, AngularAccelerationUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, AngularMomentumUnit> times(AngularMomentum multiplier) {
    return (Mult<AngleUnit, AngularMomentumUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, AngularMomentumUnit> divide(AngularMomentum divisor) {
    return (Per<AngleUnit, AngularMomentumUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, AngularVelocityUnit> times(AngularVelocity multiplier) {
    return (Mult<AngleUnit, AngularVelocityUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, AngularVelocityUnit> divide(AngularVelocity divisor) {
    return (Per<AngleUnit, AngularVelocityUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, CurrentUnit> times(Current multiplier) {
    return (Mult<AngleUnit, CurrentUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, CurrentUnit> divide(Current divisor) {
    return (Per<AngleUnit, CurrentUnit>) Measure.super.divide(divisor);
  }

  @Override
  default Angle divide(Dimensionless divisor) {
    return (Angle) Radians.of(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  @Override
  default Angle times(Dimensionless multiplier) {
    return (Angle) Radians.of(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }


  @Override
  default Mult<AngleUnit, DistanceUnit> times(Distance multiplier) {
    return (Mult<AngleUnit, DistanceUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, DistanceUnit> divide(Distance divisor) {
    return (Per<AngleUnit, DistanceUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, EnergyUnit> times(Energy multiplier) {
    return (Mult<AngleUnit, EnergyUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, EnergyUnit> divide(Energy divisor) {
    return (Per<AngleUnit, EnergyUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, ForceUnit> times(Force multiplier) {
    return (Mult<AngleUnit, ForceUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, ForceUnit> divide(Force divisor) {
    return (Per<AngleUnit, ForceUnit>) Measure.super.divide(divisor);
  }


  @Override
  default AngularVelocity times(Frequency multiplier) {
    return RadiansPerSecond.of(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  @Override
  default Per<AngleUnit, FrequencyUnit> divide(Frequency divisor) {
    return (Per<AngleUnit, FrequencyUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, LinearAccelerationUnit> times(LinearAcceleration multiplier) {
    return (Mult<AngleUnit, LinearAccelerationUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, LinearAccelerationUnit> divide(LinearAcceleration divisor) {
    return (Per<AngleUnit, LinearAccelerationUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, LinearMomentumUnit> times(LinearMomentum multiplier) {
    return (Mult<AngleUnit, LinearMomentumUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, LinearMomentumUnit> divide(LinearMomentum divisor) {
    return (Per<AngleUnit, LinearMomentumUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, LinearVelocityUnit> times(LinearVelocity multiplier) {
    return (Mult<AngleUnit, LinearVelocityUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, LinearVelocityUnit> divide(LinearVelocity divisor) {
    return (Per<AngleUnit, LinearVelocityUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, MassUnit> times(Mass multiplier) {
    return (Mult<AngleUnit, MassUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, MassUnit> divide(Mass divisor) {
    return (Per<AngleUnit, MassUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, MomentOfInertiaUnit> times(MomentOfInertia multiplier) {
    return (Mult<AngleUnit, MomentOfInertiaUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, MomentOfInertiaUnit> divide(MomentOfInertia divisor) {
    return (Per<AngleUnit, MomentOfInertiaUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, MultUnit<?, ?>> times(Mult<?, ?> multiplier) {
    return (Mult<AngleUnit, MultUnit<?, ?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, MultUnit<?, ?>> divide(Mult<?, ?> divisor) {
    return (Per<AngleUnit, MultUnit<?, ?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, PerUnit<?, ?>> times(Per<?, ?> multiplier) {
    return (Mult<AngleUnit, PerUnit<?, ?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, PerUnit<?, ?>> divide(Per<?, ?> divisor) {
    return (Per<AngleUnit, PerUnit<?, ?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, PowerUnit> times(Power multiplier) {
    return (Mult<AngleUnit, PowerUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, PowerUnit> divide(Power divisor) {
    return (Per<AngleUnit, PowerUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, ResistanceUnit> times(Resistance multiplier) {
    return (Mult<AngleUnit, ResistanceUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, ResistanceUnit> divide(Resistance divisor) {
    return (Per<AngleUnit, ResistanceUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, TemperatureUnit> times(Temperature multiplier) {
    return (Mult<AngleUnit, TemperatureUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, TemperatureUnit> divide(Temperature divisor) {
    return (Per<AngleUnit, TemperatureUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, TimeUnit> times(Time multiplier) {
    return (Mult<AngleUnit, TimeUnit>) Measure.super.times(multiplier);
  }

  @Override
  default AngularVelocity divide(Time divisor) {
    return RadiansPerSecond.of(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }


  @Override
  default Mult<AngleUnit, TorqueUnit> times(Torque multiplier) {
    return (Mult<AngleUnit, TorqueUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, TorqueUnit> divide(Torque divisor) {
    return (Per<AngleUnit, TorqueUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, VelocityUnit<?>> times(Velocity<?> multiplier) {
    return (Mult<AngleUnit, VelocityUnit<?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, VelocityUnit<?>> divide(Velocity<?> divisor) {
    return (Per<AngleUnit, VelocityUnit<?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AngleUnit, VoltageUnit> times(Voltage multiplier) {
    return (Mult<AngleUnit, VoltageUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AngleUnit, VoltageUnit> divide(Voltage divisor) {
    return (Per<AngleUnit, VoltageUnit>) Measure.super.divide(divisor);
  }

}