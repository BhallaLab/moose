

#
# Generated Sun Apr 17 15:01:32 2016 by generateDS.py version 2.22a.
#
# Command line options:
#   ('-o', 'generated_neuroml.py')
#   ('-s', 'generated_neuromlsub.py')
#
# Command line arguments:
#   /home/subha/src/neuroml_dev/NeuroML2/Schemas/NeuroML2/NeuroML_v2beta.xsd
#
# Command line:
#   /home/subha/.local/bin/generateDS.py -o "generated_neuroml.py" -s "generated_neuromlsub.py" /home/subha/src/neuroml_dev/NeuroML2/Schemas/NeuroML2/NeuroML_v2beta.xsd
#
# Current working directory (os.getcwd()):
#   neuroml2
#

import sys
from lxml import etree as etree_

# FIXME: Comment it out 
# import ??? as supermod

def parsexml_(infile, parser=None, **kwargs):
    if parser is None:
        # Use the lxml ElementTree compatible parser so that, e.g.,
        #   we ignore comments.
        parser = etree_.ETCompatXMLParser()
    doc = etree_.parse(infile, parser=parser, **kwargs)
    return doc

#
# Globals
#

ExternalEncoding = 'ascii'

#
# Data representation classes
#


class AnnotationSub(supermod.Annotation):
    def __init__(self, anytypeobjs_=None):
        super(AnnotationSub, self).__init__(anytypeobjs_, )
supermod.Annotation.subclass = AnnotationSub
# end class AnnotationSub


class ComponentTypeSub(supermod.ComponentType):
    def __init__(self, name=None, extends=None, description=None, anytypeobjs_=None):
        super(ComponentTypeSub, self).__init__(name, extends, description, anytypeobjs_, )
supermod.ComponentType.subclass = ComponentTypeSub
# end class ComponentTypeSub


class IncludeTypeSub(supermod.IncludeType):
    def __init__(self, href=None, valueOf_=None, mixedclass_=None, content_=None):
        super(IncludeTypeSub, self).__init__(href, valueOf_, mixedclass_, content_, )
supermod.IncludeType.subclass = IncludeTypeSub
# end class IncludeTypeSub


class Q10SettingsSub(supermod.Q10Settings):
    def __init__(self, type_=None, fixedQ10=None, q10Factor=None, experimentalTemp=None):
        super(Q10SettingsSub, self).__init__(type_, fixedQ10, q10Factor, experimentalTemp, )
supermod.Q10Settings.subclass = Q10SettingsSub
# end class Q10SettingsSub


class HHRateSub(supermod.HHRate):
    def __init__(self, type_=None, rate=None, midpoint=None, scale=None):
        super(HHRateSub, self).__init__(type_, rate, midpoint, scale, )
supermod.HHRate.subclass = HHRateSub
# end class HHRateSub


class HHVariableSub(supermod.HHVariable):
    def __init__(self, type_=None, rate=None, midpoint=None, scale=None):
        super(HHVariableSub, self).__init__(type_, rate, midpoint, scale, )
supermod.HHVariable.subclass = HHVariableSub
# end class HHVariableSub


class HHTimeSub(supermod.HHTime):
    def __init__(self, type_=None, rate=None, midpoint=None, scale=None, tau=None):
        super(HHTimeSub, self).__init__(type_, rate, midpoint, scale, tau, )
supermod.HHTime.subclass = HHTimeSub
# end class HHTimeSub


class BlockMechanismSub(supermod.BlockMechanism):
    def __init__(self, type_=None, species=None, blockConcentration=None, scalingConc=None, scalingVolt=None):
        super(BlockMechanismSub, self).__init__(type_, species, blockConcentration, scalingConc, scalingVolt, )
supermod.BlockMechanism.subclass = BlockMechanismSub
# end class BlockMechanismSub


class PlasticityMechanismSub(supermod.PlasticityMechanism):
    def __init__(self, type_=None, initReleaseProb=None, tauRec=None, tauFac=None):
        super(PlasticityMechanismSub, self).__init__(type_, initReleaseProb, tauRec, tauFac, )
supermod.PlasticityMechanism.subclass = PlasticityMechanismSub
# end class PlasticityMechanismSub


class SegmentParentSub(supermod.SegmentParent):
    def __init__(self, segment=None, fractionAlong='1'):
        super(SegmentParentSub, self).__init__(segment, fractionAlong, )
supermod.SegmentParent.subclass = SegmentParentSub
# end class SegmentParentSub


class Point3DWithDiamSub(supermod.Point3DWithDiam):
    def __init__(self, x=None, y=None, z=None, diameter=None):
        super(Point3DWithDiamSub, self).__init__(x, y, z, diameter, )
supermod.Point3DWithDiam.subclass = Point3DWithDiamSub
# end class Point3DWithDiamSub


class ProximalDetailsSub(supermod.ProximalDetails):
    def __init__(self, translationStart=None):
        super(ProximalDetailsSub, self).__init__(translationStart, )
supermod.ProximalDetails.subclass = ProximalDetailsSub
# end class ProximalDetailsSub


class DistalDetailsSub(supermod.DistalDetails):
    def __init__(self, normalizationEnd=None):
        super(DistalDetailsSub, self).__init__(normalizationEnd, )
supermod.DistalDetails.subclass = DistalDetailsSub
# end class DistalDetailsSub


class MemberSub(supermod.Member):
    def __init__(self, segment=None):
        super(MemberSub, self).__init__(segment, )
supermod.Member.subclass = MemberSub
# end class MemberSub


class IncludeSub(supermod.Include):
    def __init__(self, segmentGroup=None):
        super(IncludeSub, self).__init__(segmentGroup, )
supermod.Include.subclass = IncludeSub
# end class IncludeSub


class PathSub(supermod.Path):
    def __init__(self, from_=None, to=None):
        super(PathSub, self).__init__(from_, to, )
supermod.Path.subclass = PathSub
# end class PathSub


class SubTreeSub(supermod.SubTree):
    def __init__(self, from_=None, to=None):
        super(SubTreeSub, self).__init__(from_, to, )
supermod.SubTree.subclass = SubTreeSub
# end class SubTreeSub


class SegmentEndPointSub(supermod.SegmentEndPoint):
    def __init__(self, segment=None):
        super(SegmentEndPointSub, self).__init__(segment, )
supermod.SegmentEndPoint.subclass = SegmentEndPointSub
# end class SegmentEndPointSub


class MembranePropertiesSub(supermod.MembraneProperties):
    def __init__(self, channelPopulation=None, channelDensity=None, spikeThresh=None, specificCapacitance=None, initMembPotential=None, reversalPotential=None):
        super(MembranePropertiesSub, self).__init__(channelPopulation, channelDensity, spikeThresh, specificCapacitance, initMembPotential, reversalPotential, )
supermod.MembraneProperties.subclass = MembranePropertiesSub
# end class MembranePropertiesSub


class ValueAcrossSegOrSegGroupSub(supermod.ValueAcrossSegOrSegGroup):
    def __init__(self, value=None, segmentGroup='all', segment=None, extensiontype_=None):
        super(ValueAcrossSegOrSegGroupSub, self).__init__(value, segmentGroup, segment, extensiontype_, )
supermod.ValueAcrossSegOrSegGroup.subclass = ValueAcrossSegOrSegGroupSub
# end class ValueAcrossSegOrSegGroupSub


class VariableParameterSub(supermod.VariableParameter):
    def __init__(self, parameter=None, segmentGroup=None, inhomogeneousValue=None):
        super(VariableParameterSub, self).__init__(parameter, segmentGroup, inhomogeneousValue, )
supermod.VariableParameter.subclass = VariableParameterSub
# end class VariableParameterSub


class InhomogeneousValueSub(supermod.InhomogeneousValue):
    def __init__(self, inhomogeneousParam=None, value=None):
        super(InhomogeneousValueSub, self).__init__(inhomogeneousParam, value, )
supermod.InhomogeneousValue.subclass = InhomogeneousValueSub
# end class InhomogeneousValueSub


class ReversalPotentialSub(supermod.ReversalPotential):
    def __init__(self, value=None, segmentGroup='all', segment=None, species=None):
        super(ReversalPotentialSub, self).__init__(value, segmentGroup, segment, species, )
supermod.ReversalPotential.subclass = ReversalPotentialSub
# end class ReversalPotentialSub


class SpeciesSub(supermod.Species):
    def __init__(self, value=None, segmentGroup='all', segment=None, id=None, concentrationModel=None, ion=None, initialConcentration=None, initialExtConcentration=None):
        super(SpeciesSub, self).__init__(value, segmentGroup, segment, id, concentrationModel, ion, initialConcentration, initialExtConcentration, )
supermod.Species.subclass = SpeciesSub
# end class SpeciesSub


class IntracellularPropertiesSub(supermod.IntracellularProperties):
    def __init__(self, species=None, resistivity=None):
        super(IntracellularPropertiesSub, self).__init__(species, resistivity, )
supermod.IntracellularProperties.subclass = IntracellularPropertiesSub
# end class IntracellularPropertiesSub


class ExtracellularPropertiesLocalSub(supermod.ExtracellularPropertiesLocal):
    def __init__(self, temperature=None, species=None):
        super(ExtracellularPropertiesLocalSub, self).__init__(temperature, species, )
supermod.ExtracellularPropertiesLocal.subclass = ExtracellularPropertiesLocalSub
# end class ExtracellularPropertiesLocalSub


class SpaceStructureSub(supermod.SpaceStructure):
    def __init__(self, xSpacing=None, ySpacing=None, zSpacing=None, xStart=0, yStart=0, zStart=0):
        super(SpaceStructureSub, self).__init__(xSpacing, ySpacing, zSpacing, xStart, yStart, zStart, )
supermod.SpaceStructure.subclass = SpaceStructureSub
# end class SpaceStructureSub


class LayoutSub(supermod.Layout):
    def __init__(self, space=None, random=None, grid=None, unstructured=None):
        super(LayoutSub, self).__init__(space, random, grid, unstructured, )
supermod.Layout.subclass = LayoutSub
# end class LayoutSub


class UnstructuredLayoutSub(supermod.UnstructuredLayout):
    def __init__(self, number=None):
        super(UnstructuredLayoutSub, self).__init__(number, )
supermod.UnstructuredLayout.subclass = UnstructuredLayoutSub
# end class UnstructuredLayoutSub


class RandomLayoutSub(supermod.RandomLayout):
    def __init__(self, number=None, region=None):
        super(RandomLayoutSub, self).__init__(number, region, )
supermod.RandomLayout.subclass = RandomLayoutSub
# end class RandomLayoutSub


class GridLayoutSub(supermod.GridLayout):
    def __init__(self, xSize=None, ySize=None, zSize=None):
        super(GridLayoutSub, self).__init__(xSize, ySize, zSize, )
supermod.GridLayout.subclass = GridLayoutSub
# end class GridLayoutSub


class InstanceSub(supermod.Instance):
    def __init__(self, id=None, i=None, j=None, k=None, location=None):
        super(InstanceSub, self).__init__(id, i, j, k, location, )
supermod.Instance.subclass = InstanceSub
# end class InstanceSub


class LocationSub(supermod.Location):
    def __init__(self, x=None, y=None, z=None):
        super(LocationSub, self).__init__(x, y, z, )
supermod.Location.subclass = LocationSub
# end class LocationSub


class SynapticConnectionSub(supermod.SynapticConnection):
    def __init__(self, from_=None, to=None, synapse=None):
        super(SynapticConnectionSub, self).__init__(from_, to, synapse, )
supermod.SynapticConnection.subclass = SynapticConnectionSub
# end class SynapticConnectionSub


class ConnectionSub(supermod.Connection):
    def __init__(self, id=None, preCellId=None, postCellId=None):
        super(ConnectionSub, self).__init__(id, preCellId, postCellId, )
supermod.Connection.subclass = ConnectionSub
# end class ConnectionSub


class ExplicitInputSub(supermod.ExplicitInput):
    def __init__(self, target=None, input=None, destination=None):
        super(ExplicitInputSub, self).__init__(target, input, destination, )
supermod.ExplicitInput.subclass = ExplicitInputSub
# end class ExplicitInputSub


class InputSub(supermod.Input):
    def __init__(self, id=None, target=None, destination=None):
        super(InputSub, self).__init__(id, target, destination, )
supermod.Input.subclass = InputSub
# end class InputSub


class BaseSub(supermod.Base):
    def __init__(self, id=None, neuroLexId=None, extensiontype_=None):
        super(BaseSub, self).__init__(id, neuroLexId, extensiontype_, )
supermod.Base.subclass = BaseSub
# end class BaseSub


class StandaloneSub(supermod.Standalone):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, extensiontype_=None):
        super(StandaloneSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, extensiontype_, )
supermod.Standalone.subclass = StandaloneSub
# end class StandaloneSub


class SpikeSourcePoissonSub(supermod.SpikeSourcePoisson):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, start=None, duration=None, rate=None):
        super(SpikeSourcePoissonSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, start, duration, rate, )
supermod.SpikeSourcePoisson.subclass = SpikeSourcePoissonSub
# end class SpikeSourcePoissonSub


class InputListSub(supermod.InputList):
    def __init__(self, id=None, neuroLexId=None, population=None, component=None, input=None):
        super(InputListSub, self).__init__(id, neuroLexId, population, component, input, )
supermod.InputList.subclass = InputListSub
# end class InputListSub


class ProjectionSub(supermod.Projection):
    def __init__(self, id=None, neuroLexId=None, presynapticPopulation=None, postsynapticPopulation=None, synapse=None, connection=None):
        super(ProjectionSub, self).__init__(id, neuroLexId, presynapticPopulation, postsynapticPopulation, synapse, connection, )
supermod.Projection.subclass = ProjectionSub
# end class ProjectionSub


class CellSetSub(supermod.CellSet):
    def __init__(self, id=None, neuroLexId=None, select=None, anytypeobjs_=None):
        super(CellSetSub, self).__init__(id, neuroLexId, select, anytypeobjs_, )
supermod.CellSet.subclass = CellSetSub
# end class CellSetSub


class PopulationSub(supermod.Population):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cell=None, network=None, component=None, size=None, type_=None, extracellularProperties=None, layout=None, instance=None):
        super(PopulationSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cell, network, component, size, type_, extracellularProperties, layout, instance, )
supermod.Population.subclass = PopulationSub
# end class PopulationSub


class RegionSub(supermod.Region):
    def __init__(self, id=None, neuroLexId=None, space=None, anytypeobjs_=None):
        super(RegionSub, self).__init__(id, neuroLexId, space, anytypeobjs_, )
supermod.Region.subclass = RegionSub
# end class RegionSub


class SpaceSub(supermod.Space):
    def __init__(self, id=None, neuroLexId=None, basedOn=None, structure=None):
        super(SpaceSub, self).__init__(id, neuroLexId, basedOn, structure, )
supermod.Space.subclass = SpaceSub
# end class SpaceSub


class NetworkSub(supermod.Network):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, space=None, region=None, extracellularProperties=None, population=None, cellSet=None, synapticConnection=None, projection=None, explicitInput=None, inputList=None):
        super(NetworkSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, space, region, extracellularProperties, population, cellSet, synapticConnection, projection, explicitInput, inputList, )
supermod.Network.subclass = NetworkSub
# end class NetworkSub


class SpikeGeneratorPoissonSub(supermod.SpikeGeneratorPoisson):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, averageRate=None):
        super(SpikeGeneratorPoissonSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, averageRate, )
supermod.SpikeGeneratorPoisson.subclass = SpikeGeneratorPoissonSub
# end class SpikeGeneratorPoissonSub


class SpikeGeneratorRandomSub(supermod.SpikeGeneratorRandom):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, maxISI=None, minISI=None):
        super(SpikeGeneratorRandomSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, maxISI, minISI, )
supermod.SpikeGeneratorRandom.subclass = SpikeGeneratorRandomSub
# end class SpikeGeneratorRandomSub


class SpikeGeneratorSub(supermod.SpikeGenerator):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, period=None):
        super(SpikeGeneratorSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, period, )
supermod.SpikeGenerator.subclass = SpikeGeneratorSub
# end class SpikeGeneratorSub


class SpikeArraySub(supermod.SpikeArray):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, spike=None):
        super(SpikeArraySub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, spike, )
supermod.SpikeArray.subclass = SpikeArraySub
# end class SpikeArraySub


class SpikeSub(supermod.Spike):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, time=None):
        super(SpikeSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, time, )
supermod.Spike.subclass = SpikeSub
# end class SpikeSub


class VoltageClampSub(supermod.VoltageClamp):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, delay=None, duration=None, targetVoltage=None, seriesResistance=None):
        super(VoltageClampSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, delay, duration, targetVoltage, seriesResistance, )
supermod.VoltageClamp.subclass = VoltageClampSub
# end class VoltageClampSub


class RampGeneratorSub(supermod.RampGenerator):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, delay=None, duration=None, startAmplitude=None, finishAmplitude=None, baselineAmplitude=None):
        super(RampGeneratorSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, delay, duration, startAmplitude, finishAmplitude, baselineAmplitude, )
supermod.RampGenerator.subclass = RampGeneratorSub
# end class RampGeneratorSub


class SineGeneratorSub(supermod.SineGenerator):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, delay=None, phase=None, duration=None, amplitude=None, period=None):
        super(SineGeneratorSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, delay, phase, duration, amplitude, period, )
supermod.SineGenerator.subclass = SineGeneratorSub
# end class SineGeneratorSub


class PulseGeneratorSub(supermod.PulseGenerator):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, delay=None, duration=None, amplitude=None):
        super(PulseGeneratorSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, delay, duration, amplitude, )
supermod.PulseGenerator.subclass = PulseGeneratorSub
# end class PulseGeneratorSub


class ReactionSchemeSub(supermod.ReactionScheme):
    def __init__(self, id=None, neuroLexId=None, source=None, type_=None, anytypeobjs_=None):
        super(ReactionSchemeSub, self).__init__(id, neuroLexId, source, type_, anytypeobjs_, )
supermod.ReactionScheme.subclass = ReactionSchemeSub
# end class ReactionSchemeSub


class ExtracellularPropertiesSub(supermod.ExtracellularProperties):
    def __init__(self, id=None, neuroLexId=None, temperature=None, species=None):
        super(ExtracellularPropertiesSub, self).__init__(id, neuroLexId, temperature, species, )
supermod.ExtracellularProperties.subclass = ExtracellularPropertiesSub
# end class ExtracellularPropertiesSub


class ChannelDensitySub(supermod.ChannelDensity):
    def __init__(self, id=None, neuroLexId=None, ionChannel=None, condDensity=None, erev=None, segmentGroup='all', segment=None, ion=None, variableParameter=None):
        super(ChannelDensitySub, self).__init__(id, neuroLexId, ionChannel, condDensity, erev, segmentGroup, segment, ion, variableParameter, )
supermod.ChannelDensity.subclass = ChannelDensitySub
# end class ChannelDensitySub


class ChannelPopulationSub(supermod.ChannelPopulation):
    def __init__(self, id=None, neuroLexId=None, ionChannel=None, number=None, erev=None, segmentGroup='all', segment=None, ion=None, variableParameter=None):
        super(ChannelPopulationSub, self).__init__(id, neuroLexId, ionChannel, number, erev, segmentGroup, segment, ion, variableParameter, )
supermod.ChannelPopulation.subclass = ChannelPopulationSub
# end class ChannelPopulationSub


class BiophysicalPropertiesSub(supermod.BiophysicalProperties):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, membraneProperties=None, intracellularProperties=None, extracellularProperties=None):
        super(BiophysicalPropertiesSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, membraneProperties, intracellularProperties, extracellularProperties, )
supermod.BiophysicalProperties.subclass = BiophysicalPropertiesSub
# end class BiophysicalPropertiesSub


class InhomogeneousParamSub(supermod.InhomogeneousParam):
    def __init__(self, id=None, neuroLexId=None, variable=None, metric=None, proximal=None, distal=None):
        super(InhomogeneousParamSub, self).__init__(id, neuroLexId, variable, metric, proximal, distal, )
supermod.InhomogeneousParam.subclass = InhomogeneousParamSub
# end class InhomogeneousParamSub


class SegmentGroupSub(supermod.SegmentGroup):
    def __init__(self, id=None, neuroLexId=None, member=None, include=None, path=None, subTree=None, inhomogeneousParam=None):
        super(SegmentGroupSub, self).__init__(id, neuroLexId, member, include, path, subTree, inhomogeneousParam, )
supermod.SegmentGroup.subclass = SegmentGroupSub
# end class SegmentGroupSub


class SegmentSub(supermod.Segment):
    def __init__(self, id=None, neuroLexId=None, name=None, parent=None, proximal=None, distal=None):
        super(SegmentSub, self).__init__(id, neuroLexId, name, parent, proximal, distal, )
supermod.Segment.subclass = SegmentSub
# end class SegmentSub


class MorphologySub(supermod.Morphology):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, segment=None, segmentGroup=None):
        super(MorphologySub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, segment, segmentGroup, )
supermod.Morphology.subclass = MorphologySub
# end class MorphologySub


class BaseCellSub(supermod.BaseCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, extensiontype_=None):
        super(BaseCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, extensiontype_, )
supermod.BaseCell.subclass = BaseCellSub
# end class BaseCellSub


class BaseSynapseSub(supermod.BaseSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, extensiontype_=None):
        super(BaseSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, extensiontype_, )
supermod.BaseSynapse.subclass = BaseSynapseSub
# end class BaseSynapseSub


class DecayingPoolConcentrationModelSub(supermod.DecayingPoolConcentrationModel):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, ion=None, restingConc=None, decayConstant=None, shellThickness=None, extensiontype_=None):
        super(DecayingPoolConcentrationModelSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, ion, restingConc, decayConstant, shellThickness, extensiontype_, )
supermod.DecayingPoolConcentrationModel.subclass = DecayingPoolConcentrationModelSub
# end class DecayingPoolConcentrationModelSub


class GateHHRatesInfSub(supermod.GateHHRatesInf):
    def __init__(self, id=None, neuroLexId=None, instances=1, type_=None, notes=None, q10Settings=None, forwardRate=None, reverseRate=None, steadyState=None):
        super(GateHHRatesInfSub, self).__init__(id, neuroLexId, instances, type_, notes, q10Settings, forwardRate, reverseRate, steadyState, )
supermod.GateHHRatesInf.subclass = GateHHRatesInfSub
# end class GateHHRatesInfSub


class GateHHRatesTauSub(supermod.GateHHRatesTau):
    def __init__(self, id=None, neuroLexId=None, instances=1, type_=None, notes=None, q10Settings=None, forwardRate=None, reverseRate=None, timeCourse=None):
        super(GateHHRatesTauSub, self).__init__(id, neuroLexId, instances, type_, notes, q10Settings, forwardRate, reverseRate, timeCourse, )
supermod.GateHHRatesTau.subclass = GateHHRatesTauSub
# end class GateHHRatesTauSub


class GateHHTauInfSub(supermod.GateHHTauInf):
    def __init__(self, id=None, neuroLexId=None, instances=1, type_=None, notes=None, q10Settings=None, timeCourse=None, steadyState=None):
        super(GateHHTauInfSub, self).__init__(id, neuroLexId, instances, type_, notes, q10Settings, timeCourse, steadyState, )
supermod.GateHHTauInf.subclass = GateHHTauInfSub
# end class GateHHTauInfSub


class GateHHRatesSub(supermod.GateHHRates):
    def __init__(self, id=None, neuroLexId=None, instances=1, type_=None, notes=None, q10Settings=None, forwardRate=None, reverseRate=None):
        super(GateHHRatesSub, self).__init__(id, neuroLexId, instances, type_, notes, q10Settings, forwardRate, reverseRate, )
supermod.GateHHRates.subclass = GateHHRatesSub
# end class GateHHRatesSub


class GateHHUndeterminedSub(supermod.GateHHUndetermined):
    def __init__(self, id=None, neuroLexId=None, instances=1, type_=None, notes=None, q10Settings=None, forwardRate=None, reverseRate=None, timeCourse=None, steadyState=None):
        super(GateHHUndeterminedSub, self).__init__(id, neuroLexId, instances, type_, notes, q10Settings, forwardRate, reverseRate, timeCourse, steadyState, )
supermod.GateHHUndetermined.subclass = GateHHUndeterminedSub
# end class GateHHUndeterminedSub


class IonChannelSub(supermod.IonChannel):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, species=None, type_=None, conductance=None, gate=None, gateHHrates=None, gateHHratesTau=None, gateHHtauInf=None, gateHHratesInf=None):
        super(IonChannelSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, species, type_, conductance, gate, gateHHrates, gateHHratesTau, gateHHtauInf, gateHHratesInf, )
supermod.IonChannel.subclass = IonChannelSub
# end class IonChannelSub


class NeuroMLDocumentSub(supermod.NeuroMLDocument):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, include=None, extracellularProperties=None, intracellularProperties=None, morphology=None, ionChannel=None, decayingPoolConcentrationModel=None, expOneSynapse=None, expTwoSynapse=None, blockingPlasticSynapse=None, biophysicalProperties=None, cell=None, baseCell=None, iafTauCell=None, iafTauRefCell=None, iafCell=None, iafRefCell=None, izhikevichCell=None, adExIaFCell=None, pulseGenerator=None, sineGenerator=None, rampGenerator=None, voltageClamp=None, spikeArray=None, spikeGenerator=None, spikeGeneratorRandom=None, spikeGeneratorPoisson=None, IF_curr_alpha=None, IF_curr_exp=None, IF_cond_alpha=None, IF_cond_exp=None, EIF_cond_exp_isfa_ista=None, EIF_cond_alpha_isfa_ista=None, HH_cond_exp=None, expCondSynapse=None, alphaCondSynapse=None, expCurrSynapse=None, alphaCurrSynapse=None, SpikeSourcePoisson=None, network=None, ComponentType=None):
        super(NeuroMLDocumentSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, include, extracellularProperties, intracellularProperties, morphology, ionChannel, decayingPoolConcentrationModel, expOneSynapse, expTwoSynapse, blockingPlasticSynapse, biophysicalProperties, cell, baseCell, iafTauCell, iafTauRefCell, iafCell, iafRefCell, izhikevichCell, adExIaFCell, pulseGenerator, sineGenerator, rampGenerator, voltageClamp, spikeArray, spikeGenerator, spikeGeneratorRandom, spikeGeneratorPoisson, IF_curr_alpha, IF_curr_exp, IF_cond_alpha, IF_cond_exp, EIF_cond_exp_isfa_ista, EIF_cond_alpha_isfa_ista, HH_cond_exp, expCondSynapse, alphaCondSynapse, expCurrSynapse, alphaCurrSynapse, SpikeSourcePoisson, network, ComponentType, )
supermod.NeuroMLDocument.subclass = NeuroMLDocumentSub
# end class NeuroMLDocumentSub


class BasePynnSynapseSub(supermod.BasePynnSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn=None, extensiontype_=None):
        super(BasePynnSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn, extensiontype_, )
supermod.BasePynnSynapse.subclass = BasePynnSynapseSub
# end class BasePynnSynapseSub


class basePyNNCellSub(supermod.basePyNNCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, extensiontype_=None):
        super(basePyNNCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, extensiontype_, )
supermod.basePyNNCell.subclass = basePyNNCellSub
# end class basePyNNCellSub


class ConcentrationModel_DSub(supermod.ConcentrationModel_D):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, ion=None, restingConc=None, decayConstant=None, shellThickness=None, type_=None):
        super(ConcentrationModel_DSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, ion, restingConc, decayConstant, shellThickness, type_, )
supermod.ConcentrationModel_D.subclass = ConcentrationModel_DSub
# end class ConcentrationModel_DSub


class CellSub(supermod.Cell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, morphology_attr=None, biophysicalProperties_attr=None, morphology=None, biophysicalProperties=None):
        super(CellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, morphology_attr, biophysicalProperties_attr, morphology, biophysicalProperties, )
supermod.Cell.subclass = CellSub
# end class CellSub


class AdExIaFCellSub(supermod.AdExIaFCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, C=None, gL=None, EL=None, reset=None, VT=None, thresh=None, delT=None, tauw=None, refract=None, a=None, b=None):
        super(AdExIaFCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, C, gL, EL, reset, VT, thresh, delT, tauw, refract, a, b, )
supermod.AdExIaFCell.subclass = AdExIaFCellSub
# end class AdExIaFCellSub


class IzhikevichCellSub(supermod.IzhikevichCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, v0=None, thresh=None, a=None, b=None, c=None, d=None):
        super(IzhikevichCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, v0, thresh, a, b, c, d, )
supermod.IzhikevichCell.subclass = IzhikevichCellSub
# end class IzhikevichCellSub


class IaFCellSub(supermod.IaFCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, leakReversal=None, thresh=None, reset=None, C=None, leakConductance=None, extensiontype_=None):
        super(IaFCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, leakReversal, thresh, reset, C, leakConductance, extensiontype_, )
supermod.IaFCell.subclass = IaFCellSub
# end class IaFCellSub


class IaFTauCellSub(supermod.IaFTauCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, leakReversal=None, thresh=None, reset=None, tau=None, extensiontype_=None):
        super(IaFTauCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, leakReversal, thresh, reset, tau, extensiontype_, )
supermod.IaFTauCell.subclass = IaFTauCellSub
# end class IaFTauCellSub


class BaseConductanceBasedSynapseSub(supermod.BaseConductanceBasedSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, gbase=None, erev=None, extensiontype_=None):
        super(BaseConductanceBasedSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, gbase, erev, extensiontype_, )
supermod.BaseConductanceBasedSynapse.subclass = BaseConductanceBasedSynapseSub
# end class BaseConductanceBasedSynapseSub


class AlphaCurrSynapseSub(supermod.AlphaCurrSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn=None):
        super(AlphaCurrSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn, )
supermod.AlphaCurrSynapse.subclass = AlphaCurrSynapseSub
# end class AlphaCurrSynapseSub


class ExpCurrSynapseSub(supermod.ExpCurrSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn=None):
        super(ExpCurrSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn, )
supermod.ExpCurrSynapse.subclass = ExpCurrSynapseSub
# end class ExpCurrSynapseSub


class AlphaCondSynapseSub(supermod.AlphaCondSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn=None, e_rev=None):
        super(AlphaCondSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn, e_rev, )
supermod.AlphaCondSynapse.subclass = AlphaCondSynapseSub
# end class AlphaCondSynapseSub


class ExpCondSynapseSub(supermod.ExpCondSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn=None, e_rev=None):
        super(ExpCondSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn, e_rev, )
supermod.ExpCondSynapse.subclass = ExpCondSynapseSub
# end class ExpCondSynapseSub


class HH_cond_expSub(supermod.HH_cond_exp):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, v_offset=None, e_rev_E=None, e_rev_I=None, e_rev_K=None, e_rev_Na=None, e_rev_leak=None, g_leak=None, gbar_K=None, gbar_Na=None):
        super(HH_cond_expSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, v_offset, e_rev_E, e_rev_I, e_rev_K, e_rev_Na, e_rev_leak, g_leak, gbar_K, gbar_Na, )
supermod.HH_cond_exp.subclass = HH_cond_expSub
# end class HH_cond_expSub


class basePyNNIaFCellSub(supermod.basePyNNIaFCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None, extensiontype_=None):
        super(basePyNNIaFCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, extensiontype_, )
supermod.basePyNNIaFCell.subclass = basePyNNIaFCellSub
# end class basePyNNIaFCellSub


class IaFRefCellSub(supermod.IaFRefCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, leakReversal=None, thresh=None, reset=None, C=None, leakConductance=None, refract=None):
        super(IaFRefCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, leakReversal, thresh, reset, C, leakConductance, refract, )
supermod.IaFRefCell.subclass = IaFRefCellSub
# end class IaFRefCellSub


class IaFTauRefCellSub(supermod.IaFTauRefCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, leakReversal=None, thresh=None, reset=None, tau=None, refract=None):
        super(IaFTauRefCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, leakReversal, thresh, reset, tau, refract, )
supermod.IaFTauRefCell.subclass = IaFTauRefCellSub
# end class IaFTauRefCellSub


class ExpTwoSynapseSub(supermod.ExpTwoSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, gbase=None, erev=None, tauDecay=None, tauRise=None, extensiontype_=None):
        super(ExpTwoSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, gbase, erev, tauDecay, tauRise, extensiontype_, )
supermod.ExpTwoSynapse.subclass = ExpTwoSynapseSub
# end class ExpTwoSynapseSub


class ExpOneSynapseSub(supermod.ExpOneSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, gbase=None, erev=None, tauDecay=None):
        super(ExpOneSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, gbase, erev, tauDecay, )
supermod.ExpOneSynapse.subclass = ExpOneSynapseSub
# end class ExpOneSynapseSub


class IF_curr_expSub(supermod.IF_curr_exp):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None):
        super(IF_curr_expSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, )
supermod.IF_curr_exp.subclass = IF_curr_expSub
# end class IF_curr_expSub


class IF_curr_alphaSub(supermod.IF_curr_alpha):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None):
        super(IF_curr_alphaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, )
supermod.IF_curr_alpha.subclass = IF_curr_alphaSub
# end class IF_curr_alphaSub


class basePyNNIaFCondCellSub(supermod.basePyNNIaFCondCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None, e_rev_E=None, e_rev_I=None, extensiontype_=None):
        super(basePyNNIaFCondCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, e_rev_E, e_rev_I, extensiontype_, )
supermod.basePyNNIaFCondCell.subclass = basePyNNIaFCondCellSub
# end class basePyNNIaFCondCellSub


class BlockingPlasticSynapseSub(supermod.BlockingPlasticSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, gbase=None, erev=None, tauDecay=None, tauRise=None, plasticityMechanism=None, blockMechanism=None):
        super(BlockingPlasticSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, gbase, erev, tauDecay, tauRise, plasticityMechanism, blockMechanism, )
supermod.BlockingPlasticSynapse.subclass = BlockingPlasticSynapseSub
# end class BlockingPlasticSynapseSub


class EIF_cond_alpha_isfa_istaSub(supermod.EIF_cond_alpha_isfa_ista):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None, e_rev_E=None, e_rev_I=None, a=None, b=None, delta_T=None, tau_w=None, v_spike=None):
        super(EIF_cond_alpha_isfa_istaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, e_rev_E, e_rev_I, a, b, delta_T, tau_w, v_spike, )
supermod.EIF_cond_alpha_isfa_ista.subclass = EIF_cond_alpha_isfa_istaSub
# end class EIF_cond_alpha_isfa_istaSub


class EIF_cond_exp_isfa_istaSub(supermod.EIF_cond_exp_isfa_ista):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None, e_rev_E=None, e_rev_I=None, a=None, b=None, delta_T=None, tau_w=None, v_spike=None):
        super(EIF_cond_exp_isfa_istaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, e_rev_E, e_rev_I, a, b, delta_T, tau_w, v_spike, )
supermod.EIF_cond_exp_isfa_ista.subclass = EIF_cond_exp_isfa_istaSub
# end class EIF_cond_exp_isfa_istaSub


class IF_cond_expSub(supermod.IF_cond_exp):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None, e_rev_E=None, e_rev_I=None):
        super(IF_cond_expSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, e_rev_E, e_rev_I, )
supermod.IF_cond_exp.subclass = IF_cond_expSub
# end class IF_cond_expSub


class IF_cond_alphaSub(supermod.IF_cond_alpha):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, cm=None, i_offset=None, tau_syn_E=None, tau_syn_I=None, v_init=None, tau_m=None, tau_refrac=None, v_reset=None, v_rest=None, v_thresh=None, e_rev_E=None, e_rev_I=None):
        super(IF_cond_alphaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, cm, i_offset, tau_syn_E, tau_syn_I, v_init, tau_m, tau_refrac, v_reset, v_rest, v_thresh, e_rev_E, e_rev_I, )
supermod.IF_cond_alpha.subclass = IF_cond_alphaSub
# end class IF_cond_alphaSub


def get_root_tag(node):
    tag = supermod.Tag_pattern_.match(node.tag).groups()[-1]
    rootClass = None
    rootClass = supermod.GDSClassesMapping.get(tag)
    if rootClass is None and hasattr(supermod, tag):
        rootClass = getattr(supermod, tag)
    return tag, rootClass


def parse(inFilename, silence=False):
    parser = None
    doc = parsexml_(inFilename, parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'Annotation'
        rootClass = supermod.Annotation
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    if not silence:
        sys.stdout.write('<?xml version="1.0" ?>\n')
        rootObj.export(
            sys.stdout, 0, name_=rootTag,
            namespacedef_='',
            pretty_print=True)
    return rootObj


def parseEtree(inFilename, silence=False):
    parser = None
    doc = parsexml_(inFilename, parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'Annotation'
        rootClass = supermod.Annotation
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    mapping = {}
    rootElement = rootObj.to_etree(None, name_=rootTag, mapping_=mapping)
    reverse_mapping = rootObj.gds_reverse_node_mapping(mapping)
    if not silence:
        content = etree_.tostring(
            rootElement, pretty_print=True,
            xml_declaration=True, encoding="utf-8")
        sys.stdout.write(content)
        sys.stdout.write('\n')
    return rootObj, rootElement, mapping, reverse_mapping


def parseString(inString, silence=False):
    from StringIO import StringIO
    parser = None
    doc = parsexml_(StringIO(inString), parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'Annotation'
        rootClass = supermod.Annotation
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    if not silence:
        sys.stdout.write('<?xml version="1.0" ?>\n')
        rootObj.export(
            sys.stdout, 0, name_=rootTag,
            namespacedef_='')
    return rootObj


def parseLiteral(inFilename, silence=False):
    parser = None
    doc = parsexml_(inFilename, parser)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'Annotation'
        rootClass = supermod.Annotation
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    if not silence:
        sys.stdout.write('#from ??? import *\n\n')
        sys.stdout.write('import ??? as model_\n\n')
        sys.stdout.write('rootObj = model_.rootClass(\n')
        rootObj.exportLiteral(sys.stdout, 0, name_=rootTag)
        sys.stdout.write(')\n')
    return rootObj


USAGE_TEXT = """
Usage: python ???.py <infilename>
"""


def usage():
    print(USAGE_TEXT)
    sys.exit(1)


def main():
    args = sys.argv[1:]
    if len(args) != 1:
        usage()
    infilename = args[0]
    parse(infilename)


if __name__ == '__main__':
    #import pdb; pdb.set_trace()
    main()
