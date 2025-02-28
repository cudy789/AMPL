// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// THIS FILE WAS AUTO-GENERATED BY ./wpiunits/generate_units.py. DO NOT MODIFY

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.*;
import edu.wpi.first.units.*;

@SuppressWarnings({"unchecked", "cast", "checkstyle", "PMD"})
public interface Acceleration<D extends Unit> extends Measure<AccelerationUnit<D>> {
  static <D extends Unit> Acceleration<D> ofRelativeUnits(double magnitude, AccelerationUnit<D> unit) {
    return new ImmutableAcceleration<D>(magnitude, unit.toBaseUnits(magnitude), unit);
  }

  static <D extends Unit> Acceleration<D> ofBaseUnits(double baseUnitMagnitude, AccelerationUnit<D> unit) {
    return new ImmutableAcceleration<D>(unit.fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, unit);
  }

  @Override
  Acceleration<D> copy();

  @Override
  default MutAcceleration<D> mutableCopy() {
    return new MutAcceleration<D>(magnitude(), baseUnitMagnitude(), unit());
  }

  @Override
  AccelerationUnit<D> unit();

  @Override
  default AccelerationUnit<D> baseUnit() { return (AccelerationUnit<D>) unit().getBaseUnit(); }

  @Override
  default double in(AccelerationUnit<D> unit) {
    return unit.fromBaseUnits(baseUnitMagnitude());
  }

  @Override
  default Acceleration<D> unaryMinus() {
    return (Acceleration<D>) unit().ofBaseUnits(0 - baseUnitMagnitude());
  }

  @Override
  @Deprecated(since = "2025", forRemoval = true)
  @SuppressWarnings({"deprecation", "removal"})
  default Acceleration<D> negate() {
    return (Acceleration<D>) unaryMinus();
  }

  @Override
  default Acceleration<D> plus(Measure<? extends AccelerationUnit<D>> other) {
    return (Acceleration<D>) unit().ofBaseUnits(baseUnitMagnitude() + other.baseUnitMagnitude());
  }

  @Override
  default Acceleration<D> minus(Measure<? extends AccelerationUnit<D>> other) {
    return (Acceleration<D>) unit().ofBaseUnits(baseUnitMagnitude() - other.baseUnitMagnitude());
  }

  @Override
  default Acceleration<D> times(double multiplier) {
    return (Acceleration<D>) unit().ofBaseUnits(baseUnitMagnitude() * multiplier);
  }

  @Override
  default Acceleration<D> divide(double divisor) {
    return (Acceleration<D>) unit().ofBaseUnits(baseUnitMagnitude() / divisor);
  }

  @Override
  default Velocity<AccelerationUnit<D>> per(TimeUnit period) {
    return divide(period.of(1));
  }


  @Override
  default Mult<AccelerationUnit<D>, AccelerationUnit<?>> times(Acceleration<?> multiplier) {
    return (Mult<AccelerationUnit<D>, AccelerationUnit<?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, AccelerationUnit<?>> divide(Acceleration<?> divisor) {
    return (Per<AccelerationUnit<D>, AccelerationUnit<?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, AngleUnit> times(Angle multiplier) {
    return (Mult<AccelerationUnit<D>, AngleUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, AngleUnit> divide(Angle divisor) {
    return (Per<AccelerationUnit<D>, AngleUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, AngularAccelerationUnit> times(AngularAcceleration multiplier) {
    return (Mult<AccelerationUnit<D>, AngularAccelerationUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, AngularAccelerationUnit> divide(AngularAcceleration divisor) {
    return (Per<AccelerationUnit<D>, AngularAccelerationUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, AngularMomentumUnit> times(AngularMomentum multiplier) {
    return (Mult<AccelerationUnit<D>, AngularMomentumUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, AngularMomentumUnit> divide(AngularMomentum divisor) {
    return (Per<AccelerationUnit<D>, AngularMomentumUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, AngularVelocityUnit> times(AngularVelocity multiplier) {
    return (Mult<AccelerationUnit<D>, AngularVelocityUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, AngularVelocityUnit> divide(AngularVelocity divisor) {
    return (Per<AccelerationUnit<D>, AngularVelocityUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, CurrentUnit> times(Current multiplier) {
    return (Mult<AccelerationUnit<D>, CurrentUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, CurrentUnit> divide(Current divisor) {
    return (Per<AccelerationUnit<D>, CurrentUnit>) Measure.super.divide(divisor);
  }

  @Override
  default Acceleration<D> divide(Dimensionless divisor) {
    return (Acceleration<D>) unit().of(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  @Override
  default Acceleration<D> times(Dimensionless multiplier) {
    return (Acceleration<D>) unit().of(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }


  @Override
  default Mult<AccelerationUnit<D>, DistanceUnit> times(Distance multiplier) {
    return (Mult<AccelerationUnit<D>, DistanceUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, DistanceUnit> divide(Distance divisor) {
    return (Per<AccelerationUnit<D>, DistanceUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, EnergyUnit> times(Energy multiplier) {
    return (Mult<AccelerationUnit<D>, EnergyUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, EnergyUnit> divide(Energy divisor) {
    return (Per<AccelerationUnit<D>, EnergyUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, ForceUnit> times(Force multiplier) {
    return (Mult<AccelerationUnit<D>, ForceUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, ForceUnit> divide(Force divisor) {
    return (Per<AccelerationUnit<D>, ForceUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, FrequencyUnit> times(Frequency multiplier) {
    return (Mult<AccelerationUnit<D>, FrequencyUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, FrequencyUnit> divide(Frequency divisor) {
    return (Per<AccelerationUnit<D>, FrequencyUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, LinearAccelerationUnit> times(LinearAcceleration multiplier) {
    return (Mult<AccelerationUnit<D>, LinearAccelerationUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, LinearAccelerationUnit> divide(LinearAcceleration divisor) {
    return (Per<AccelerationUnit<D>, LinearAccelerationUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, LinearMomentumUnit> times(LinearMomentum multiplier) {
    return (Mult<AccelerationUnit<D>, LinearMomentumUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, LinearMomentumUnit> divide(LinearMomentum divisor) {
    return (Per<AccelerationUnit<D>, LinearMomentumUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, LinearVelocityUnit> times(LinearVelocity multiplier) {
    return (Mult<AccelerationUnit<D>, LinearVelocityUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, LinearVelocityUnit> divide(LinearVelocity divisor) {
    return (Per<AccelerationUnit<D>, LinearVelocityUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, MassUnit> times(Mass multiplier) {
    return (Mult<AccelerationUnit<D>, MassUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, MassUnit> divide(Mass divisor) {
    return (Per<AccelerationUnit<D>, MassUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, MomentOfInertiaUnit> times(MomentOfInertia multiplier) {
    return (Mult<AccelerationUnit<D>, MomentOfInertiaUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, MomentOfInertiaUnit> divide(MomentOfInertia divisor) {
    return (Per<AccelerationUnit<D>, MomentOfInertiaUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, MultUnit<?, ?>> times(Mult<?, ?> multiplier) {
    return (Mult<AccelerationUnit<D>, MultUnit<?, ?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, MultUnit<?, ?>> divide(Mult<?, ?> divisor) {
    return (Per<AccelerationUnit<D>, MultUnit<?, ?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, PerUnit<?, ?>> times(Per<?, ?> multiplier) {
    return (Mult<AccelerationUnit<D>, PerUnit<?, ?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, PerUnit<?, ?>> divide(Per<?, ?> divisor) {
    return (Per<AccelerationUnit<D>, PerUnit<?, ?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, PowerUnit> times(Power multiplier) {
    return (Mult<AccelerationUnit<D>, PowerUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, PowerUnit> divide(Power divisor) {
    return (Per<AccelerationUnit<D>, PowerUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, ResistanceUnit> times(Resistance multiplier) {
    return (Mult<AccelerationUnit<D>, ResistanceUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, ResistanceUnit> divide(Resistance divisor) {
    return (Per<AccelerationUnit<D>, ResistanceUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, TemperatureUnit> times(Temperature multiplier) {
    return (Mult<AccelerationUnit<D>, TemperatureUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, TemperatureUnit> divide(Temperature divisor) {
    return (Per<AccelerationUnit<D>, TemperatureUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, TimeUnit> times(Time multiplier) {
    return (Mult<AccelerationUnit<D>, TimeUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Velocity<AccelerationUnit<D>> divide(Time divisor) {
    return VelocityUnit.combine(unit(), divisor.unit()).ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }


  @Override
  default Mult<AccelerationUnit<D>, TorqueUnit> times(Torque multiplier) {
    return (Mult<AccelerationUnit<D>, TorqueUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, TorqueUnit> divide(Torque divisor) {
    return (Per<AccelerationUnit<D>, TorqueUnit>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, VelocityUnit<?>> times(Velocity<?> multiplier) {
    return (Mult<AccelerationUnit<D>, VelocityUnit<?>>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, VelocityUnit<?>> divide(Velocity<?> divisor) {
    return (Per<AccelerationUnit<D>, VelocityUnit<?>>) Measure.super.divide(divisor);
  }


  @Override
  default Mult<AccelerationUnit<D>, VoltageUnit> times(Voltage multiplier) {
    return (Mult<AccelerationUnit<D>, VoltageUnit>) Measure.super.times(multiplier);
  }

  @Override
  default Per<AccelerationUnit<D>, VoltageUnit> divide(Voltage divisor) {
    return (Per<AccelerationUnit<D>, VoltageUnit>) Measure.super.divide(divisor);
  }

}
