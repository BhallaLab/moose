#!/usr/bin/env python

#
# Generated Sun Jul 28 10:18:38 2013 by generateDS.py version 2.10a.
#

import sys

etree_ = None
Verbose_import_ = False
(
    XMLParser_import_none, XMLParser_import_lxml,
    XMLParser_import_elementtree
) = range(3)
XMLParser_import_library = None
try:
    # lxml
    from lxml import etree as etree_
    XMLParser_import_library = XMLParser_import_lxml
    if Verbose_import_:
        print("running with lxml.etree")
except ImportError:
    try:
        # cElementTree from Python 2.5+
        import xml.etree.cElementTree as etree_
        XMLParser_import_library = XMLParser_import_elementtree
        if Verbose_import_:
            print("running with cElementTree on Python 2.5+")
    except ImportError:
        try:
            # ElementTree from Python 2.5+
            import xml.etree.ElementTree as etree_
            XMLParser_import_library = XMLParser_import_elementtree
            if Verbose_import_:
                print("running with ElementTree on Python 2.5+")
        except ImportError:
            try:
                # normal cElementTree install
                import cElementTree as etree_
                XMLParser_import_library = XMLParser_import_elementtree
                if Verbose_import_:
                    print("running with cElementTree")
            except ImportError:
                try:
                    # normal ElementTree install
                    import elementtree.ElementTree as etree_
                    XMLParser_import_library = XMLParser_import_elementtree
                    if Verbose_import_:
                        print("running with ElementTree")
                except ImportError:
                    raise ImportError(
                        "Failed to import ElementTree from any known place")


def parsexml_(*args, **kwargs):
    if (XMLParser_import_library == XMLParser_import_lxml and
            'parser' not in kwargs):
        # Use the lxml ElementTree compatible parser so that, e.g.,
        #   we ignore comments.
        kwargs['parser'] = etree_.ETCompatXMLParser()
    doc = etree_.parse(*args, **kwargs)
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
    def __init__(self, extends=None, name=None, description=None, anytypeobjs_=None):
        super(ComponentTypeSub, self).__init__(extends, name, description, anytypeobjs_, )
supermod.ComponentType.subclass = ComponentTypeSub
# end class ComponentTypeSub


class IncludeTypeSub(supermod.IncludeType):
    def __init__(self, href=None, valueOf_=None, mixedclass_=None, content_=None):
        super(IncludeTypeSub, self).__init__(href, valueOf_, mixedclass_, content_, )
supermod.IncludeType.subclass = IncludeTypeSub
# end class IncludeTypeSub


class Q10SettingsSub(supermod.Q10Settings):
    def __init__(self, fixedQ10=None, experimentalTemp=None, type_=None, q10Factor=None):
        super(Q10SettingsSub, self).__init__(fixedQ10, experimentalTemp, type_, q10Factor, )
supermod.Q10Settings.subclass = Q10SettingsSub
# end class Q10SettingsSub


class HHRateSub(supermod.HHRate):
    def __init__(self, midpoint=None, rate=None, scale=None, type_=None):
        super(HHRateSub, self).__init__(midpoint, rate, scale, type_, )
supermod.HHRate.subclass = HHRateSub
# end class HHRateSub


class HHVariableSub(supermod.HHVariable):
    def __init__(self, midpoint=None, rate=None, scale=None, type_=None):
        super(HHVariableSub, self).__init__(midpoint, rate, scale, type_, )
supermod.HHVariable.subclass = HHVariableSub
# end class HHVariableSub


class HHTimeSub(supermod.HHTime):
    def __init__(self, midpoint=None, rate=None, scale=None, type_=None, tau=None):
        super(HHTimeSub, self).__init__(midpoint, rate, scale, type_, tau, )
supermod.HHTime.subclass = HHTimeSub
# end class HHTimeSub


class BlockMechanismSub(supermod.BlockMechanism):
    def __init__(self, blockConcentration=None, scalingConc=None, type_=None, species=None, scalingVolt=None):
        super(BlockMechanismSub, self).__init__(blockConcentration, scalingConc, type_, species, scalingVolt, )
supermod.BlockMechanism.subclass = BlockMechanismSub
# end class BlockMechanismSub


class PlasticityMechanismSub(supermod.PlasticityMechanism):
    def __init__(self, type_=None, tauFac=None, tauRec=None, initReleaseProb=None):
        super(PlasticityMechanismSub, self).__init__(type_, tauFac, tauRec, initReleaseProb, )
supermod.PlasticityMechanism.subclass = PlasticityMechanismSub
# end class PlasticityMechanismSub


class SegmentParentSub(supermod.SegmentParent):
    def __init__(self, fractionAlong='1', segment=None):
        super(SegmentParentSub, self).__init__(fractionAlong, segment, )
supermod.SegmentParent.subclass = SegmentParentSub
# end class SegmentParentSub


class Point3DWithDiamSub(supermod.Point3DWithDiam):
    def __init__(self, y=None, x=None, z=None, diameter=None):
        super(Point3DWithDiamSub, self).__init__(y, x, z, diameter, )
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
    def __init__(self, fromxx=None, to=None):
        super(PathSub, self).__init__(fromxx, to, )
supermod.Path.subclass = PathSub
# end class PathSub


class SubTreeSub(supermod.SubTree):
    def __init__(self, fromxx=None, to=None):
        super(SubTreeSub, self).__init__(fromxx, to, )
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
    def __init__(self, segment=None, segmentGroup='all', value=None, extensiontype_=None):
        super(ValueAcrossSegOrSegGroupSub, self).__init__(segment, segmentGroup, value, extensiontype_, )
supermod.ValueAcrossSegOrSegGroup.subclass = ValueAcrossSegOrSegGroupSub
# end class ValueAcrossSegOrSegGroupSub


class VariableParameterSub(supermod.VariableParameter):
    def __init__(self, segmentGroup=None, parameter=None, inhomogeneousValue=None):
        super(VariableParameterSub, self).__init__(segmentGroup, parameter, inhomogeneousValue, )
supermod.VariableParameter.subclass = VariableParameterSub
# end class VariableParameterSub


class InhomogeneousValueSub(supermod.InhomogeneousValue):
    def __init__(self, inhomogeneousParam=None, value=None):
        super(InhomogeneousValueSub, self).__init__(inhomogeneousParam, value, )
supermod.InhomogeneousValue.subclass = InhomogeneousValueSub
# end class InhomogeneousValueSub


class ReversalPotentialSub(supermod.ReversalPotential):
    def __init__(self, segment=None, segmentGroup='all', value=None, species=None):
        super(ReversalPotentialSub, self).__init__(segment, segmentGroup, value, species, )
supermod.ReversalPotential.subclass = ReversalPotentialSub
# end class ReversalPotentialSub


class SpeciesSub(supermod.Species):
    def __init__(self, segment=None, segmentGroup='all', value=None, ion=None, initialExtConcentration=None, concentrationModel=None, id=None, initialConcentration=None):
        super(SpeciesSub, self).__init__(segment, segmentGroup, value, ion, initialExtConcentration, concentrationModel, id, initialConcentration, )
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
    def __init__(self, ySpacing=None, zStart=0, yStart=0, zSpacing=None, xStart=0, xSpacing=None):
        super(SpaceStructureSub, self).__init__(ySpacing, zStart, yStart, zSpacing, xStart, xSpacing, )
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
    def __init__(self, region=None, number=None):
        super(RandomLayoutSub, self).__init__(region, number, )
supermod.RandomLayout.subclass = RandomLayoutSub
# end class RandomLayoutSub


class GridLayoutSub(supermod.GridLayout):
    def __init__(self, zSize=None, ySize=None, xSize=None):
        super(GridLayoutSub, self).__init__(zSize, ySize, xSize, )
supermod.GridLayout.subclass = GridLayoutSub
# end class GridLayoutSub


class InstanceSub(supermod.Instance):
    def __init__(self, i=None, k=None, j=None, id=None, location=None):
        super(InstanceSub, self).__init__(i, k, j, id, location, )
supermod.Instance.subclass = InstanceSub
# end class InstanceSub


class LocationSub(supermod.Location):
    def __init__(self, y=None, x=None, z=None):
        super(LocationSub, self).__init__(y, x, z, )
supermod.Location.subclass = LocationSub
# end class LocationSub


class SynapticConnectionSub(supermod.SynapticConnection):
    def __init__(self, to=None, synapse=None, fromxx=None):
        super(SynapticConnectionSub, self).__init__(to, synapse, fromxx, )
supermod.SynapticConnection.subclass = SynapticConnectionSub
# end class SynapticConnectionSub


class ConnectionSub(supermod.Connection):
    def __init__(self, postCellId=None, id=None, preCellId=None):
        super(ConnectionSub, self).__init__(postCellId, id, preCellId, )
supermod.Connection.subclass = ConnectionSub
# end class ConnectionSub


class ExplicitInputSub(supermod.ExplicitInput):
    def __init__(self, input=None, destination=None, target=None):
        super(ExplicitInputSub, self).__init__(input, destination, target, )
supermod.ExplicitInput.subclass = ExplicitInputSub
# end class ExplicitInputSub


class InputSub(supermod.Input):
    def __init__(self, destination=None, id=None, target=None):
        super(InputSub, self).__init__(destination, id, target, )
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
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, duration=None, start=None, rate=None):
        super(SpikeSourcePoissonSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, duration, start, rate, )
supermod.SpikeSourcePoisson.subclass = SpikeSourcePoissonSub
# end class SpikeSourcePoissonSub


class InputListSub(supermod.InputList):
    def __init__(self, id=None, neuroLexId=None, component=None, population=None, input=None):
        super(InputListSub, self).__init__(id, neuroLexId, component, population, input, )
supermod.InputList.subclass = InputListSub
# end class InputListSub


class ProjectionSub(supermod.Projection):
    def __init__(self, id=None, neuroLexId=None, postsynapticPopulation=None, presynapticPopulation=None, synapse=None, connection=None):
        super(ProjectionSub, self).__init__(id, neuroLexId, postsynapticPopulation, presynapticPopulation, synapse, connection, )
supermod.Projection.subclass = ProjectionSub
# end class ProjectionSub


class CellSetSub(supermod.CellSet):
    def __init__(self, id=None, neuroLexId=None, select=None, anytypeobjs_=None):
        super(CellSetSub, self).__init__(id, neuroLexId, select, anytypeobjs_, )
supermod.CellSet.subclass = CellSetSub
# end class CellSetSub


class PopulationSub(supermod.Population):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, extracellularProperties=None, network=None, component=None, cell=None, type_=None, size=None, layout=None, instance=None):
        super(PopulationSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, extracellularProperties, network, component, cell, type_, size, layout, instance, )
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
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, minISI=None, maxISI=None):
        super(SpikeGeneratorRandomSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, minISI, maxISI, )
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
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, delay=None, duration=None, seriesResistance=None, targetVoltage=None):
        super(VoltageClampSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, delay, duration, seriesResistance, targetVoltage, )
supermod.VoltageClamp.subclass = VoltageClampSub
# end class VoltageClampSub


class RampGeneratorSub(supermod.RampGenerator):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, delay=None, duration=None, baselineAmplitude=None, startAmplitude=None, finishAmplitude=None):
        super(RampGeneratorSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, delay, duration, baselineAmplitude, startAmplitude, finishAmplitude, )
supermod.RampGenerator.subclass = RampGeneratorSub
# end class RampGeneratorSub


class SineGeneratorSub(supermod.SineGenerator):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, delay=None, phase=None, duration=None, period=None, amplitude=None):
        super(SineGeneratorSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, delay, phase, duration, period, amplitude, )
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
    def __init__(self, id=None, neuroLexId=None, segmentGroup='all', ion=None, ionChannel=None, erev=None, condDensity=None, segment=None, variableParameter=None):
        super(ChannelDensitySub, self).__init__(id, neuroLexId, segmentGroup, ion, ionChannel, erev, condDensity, segment, variableParameter, )
supermod.ChannelDensity.subclass = ChannelDensitySub
# end class ChannelDensitySub


class ChannelPopulationSub(supermod.ChannelPopulation):
    def __init__(self, id=None, neuroLexId=None, segmentGroup='all', ion=None, number=None, ionChannel=None, erev=None, segment=None, variableParameter=None):
        super(ChannelPopulationSub, self).__init__(id, neuroLexId, segmentGroup, ion, number, ionChannel, erev, segment, variableParameter, )
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
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, ion=None, shellThickness=None, restingConc=None, decayConstant=None, extensiontype_=None):
        super(DecayingPoolConcentrationModelSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, ion, shellThickness, restingConc, decayConstant, extensiontype_, )
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
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, conductance=None, type_=None, species=None, gate=None, gateHHrates=None, gateHHratesTau=None, gateHHtauInf=None, gateHHratesInf=None):
        super(IonChannelSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, conductance, type_, species, gate, gateHHrates, gateHHratesTau, gateHHtauInf, gateHHratesInf, )
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
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, extensiontype_=None):
        super(basePyNNCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, extensiontype_, )
supermod.basePyNNCell.subclass = basePyNNCellSub
# end class basePyNNCellSub


class ConcentrationModel_DSub(supermod.ConcentrationModel_D):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, ion=None, shellThickness=None, restingConc=None, decayConstant=None, type_=None):
        super(ConcentrationModel_DSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, ion, shellThickness, restingConc, decayConstant, type_, )
supermod.ConcentrationModel_D.subclass = ConcentrationModel_DSub
# end class ConcentrationModel_DSub


class CellSub(supermod.Cell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, biophysicalProperties_attr=None, morphology_attr=None, morphology=None, biophysicalProperties=None):
        super(CellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, biophysicalProperties_attr, morphology_attr, morphology, biophysicalProperties, )
supermod.Cell.subclass = CellSub
# end class CellSub


class AdExIaFCellSub(supermod.AdExIaFCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, reset=None, EL=None, C=None, b=None, refract=None, VT=None, delT=None, a=None, thresh=None, gL=None, tauw=None):
        super(AdExIaFCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, reset, EL, C, b, refract, VT, delT, a, thresh, gL, tauw, )
supermod.AdExIaFCell.subclass = AdExIaFCellSub
# end class AdExIaFCellSub


class IzhikevichCellSub(supermod.IzhikevichCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, a=None, c=None, b=None, d=None, v0=None, thresh=None):
        super(IzhikevichCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, a, c, b, d, v0, thresh, )
supermod.IzhikevichCell.subclass = IzhikevichCellSub
# end class IzhikevichCellSub


class IaFCellSub(supermod.IaFCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, reset=None, C=None, thresh=None, leakConductance=None, leakReversal=None, extensiontype_=None):
        super(IaFCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, reset, C, thresh, leakConductance, leakReversal, extensiontype_, )
supermod.IaFCell.subclass = IaFCellSub
# end class IaFCellSub


class IaFTauCellSub(supermod.IaFTauCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, reset=None, tau=None, thresh=None, leakReversal=None, extensiontype_=None):
        super(IaFTauCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, reset, tau, thresh, leakReversal, extensiontype_, )
supermod.IaFTauCell.subclass = IaFTauCellSub
# end class IaFTauCellSub


class BaseConductanceBasedSynapseSub(supermod.BaseConductanceBasedSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, erev=None, gbase=None, extensiontype_=None):
        super(BaseConductanceBasedSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, erev, gbase, extensiontype_, )
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
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, gbar_K=None, e_rev_E=None, g_leak=None, e_rev_Na=None, e_rev_I=None, e_rev_K=None, e_rev_leak=None, v_offset=None, gbar_Na=None):
        super(HH_cond_expSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, gbar_K, e_rev_E, g_leak, e_rev_Na, e_rev_I, e_rev_K, e_rev_leak, v_offset, gbar_Na, )
supermod.HH_cond_exp.subclass = HH_cond_expSub
# end class HH_cond_expSub


class basePyNNIaFCellSub(supermod.basePyNNIaFCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None, extensiontype_=None):
        super(basePyNNIaFCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, extensiontype_, )
supermod.basePyNNIaFCell.subclass = basePyNNIaFCellSub
# end class basePyNNIaFCellSub


class IaFRefCellSub(supermod.IaFRefCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, reset=None, C=None, thresh=None, leakConductance=None, leakReversal=None, refract=None):
        super(IaFRefCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, reset, C, thresh, leakConductance, leakReversal, refract, )
supermod.IaFRefCell.subclass = IaFRefCellSub
# end class IaFRefCellSub


class IaFTauRefCellSub(supermod.IaFTauRefCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, reset=None, tau=None, thresh=None, leakReversal=None, refract=None):
        super(IaFTauRefCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, reset, tau, thresh, leakReversal, refract, )
supermod.IaFTauRefCell.subclass = IaFTauRefCellSub
# end class IaFTauRefCellSub


class ExpTwoSynapseSub(supermod.ExpTwoSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, erev=None, gbase=None, tauDecay=None, tauRise=None, extensiontype_=None):
        super(ExpTwoSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, erev, gbase, tauDecay, tauRise, extensiontype_, )
supermod.ExpTwoSynapse.subclass = ExpTwoSynapseSub
# end class ExpTwoSynapseSub


class ExpOneSynapseSub(supermod.ExpOneSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, erev=None, gbase=None, tauDecay=None):
        super(ExpOneSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, erev, gbase, tauDecay, )
supermod.ExpOneSynapse.subclass = ExpOneSynapseSub
# end class ExpOneSynapseSub


class IF_curr_expSub(supermod.IF_curr_exp):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None):
        super(IF_curr_expSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, )
supermod.IF_curr_exp.subclass = IF_curr_expSub
# end class IF_curr_expSub


class IF_curr_alphaSub(supermod.IF_curr_alpha):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None):
        super(IF_curr_alphaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, )
supermod.IF_curr_alpha.subclass = IF_curr_alphaSub
# end class IF_curr_alphaSub


class basePyNNIaFCondCellSub(supermod.basePyNNIaFCondCell):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None, e_rev_I=None, e_rev_E=None, extensiontype_=None):
        super(basePyNNIaFCondCellSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, e_rev_I, e_rev_E, extensiontype_, )
supermod.basePyNNIaFCondCell.subclass = basePyNNIaFCondCellSub
# end class basePyNNIaFCondCellSub


class BlockingPlasticSynapseSub(supermod.BlockingPlasticSynapse):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, erev=None, gbase=None, tauDecay=None, tauRise=None, plasticityMechanism=None, blockMechanism=None):
        super(BlockingPlasticSynapseSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, erev, gbase, tauDecay, tauRise, plasticityMechanism, blockMechanism, )
supermod.BlockingPlasticSynapse.subclass = BlockingPlasticSynapseSub
# end class BlockingPlasticSynapseSub


class EIF_cond_alpha_isfa_istaSub(supermod.EIF_cond_alpha_isfa_ista):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None, e_rev_I=None, e_rev_E=None, a=None, delta_T=None, b=None, v_spike=None, tau_w=None):
        super(EIF_cond_alpha_isfa_istaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, e_rev_I, e_rev_E, a, delta_T, b, v_spike, tau_w, )
supermod.EIF_cond_alpha_isfa_ista.subclass = EIF_cond_alpha_isfa_istaSub
# end class EIF_cond_alpha_isfa_istaSub


class EIF_cond_exp_isfa_istaSub(supermod.EIF_cond_exp_isfa_ista):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None, e_rev_I=None, e_rev_E=None, a=None, delta_T=None, b=None, v_spike=None, tau_w=None):
        super(EIF_cond_exp_isfa_istaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, e_rev_I, e_rev_E, a, delta_T, b, v_spike, tau_w, )
supermod.EIF_cond_exp_isfa_ista.subclass = EIF_cond_exp_isfa_istaSub
# end class EIF_cond_exp_isfa_istaSub


class IF_cond_expSub(supermod.IF_cond_exp):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None, e_rev_I=None, e_rev_E=None):
        super(IF_cond_expSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, e_rev_I, e_rev_E, )
supermod.IF_cond_exp.subclass = IF_cond_expSub
# end class IF_cond_expSub


class IF_cond_alphaSub(supermod.IF_cond_alpha):
    def __init__(self, id=None, neuroLexId=None, name=None, metaid=None, notes=None, annotation=None, tau_syn_I=None, tau_syn_E=None, i_offset=None, cm=None, v_init=None, tau_refrac=None, v_thresh=None, tau_m=None, v_reset=None, v_rest=None, e_rev_I=None, e_rev_E=None):
        super(IF_cond_alphaSub, self).__init__(id, neuroLexId, name, metaid, notes, annotation, tau_syn_I, tau_syn_E, i_offset, cm, v_init, tau_refrac, v_thresh, tau_m, v_reset, v_rest, e_rev_I, e_rev_E, )
supermod.IF_cond_alpha.subclass = IF_cond_alphaSub
# end class IF_cond_alphaSub


def get_root_tag(node):
    tag = supermod.Tag_pattern_.match(node.tag).groups()[-1]
    rootClass = None
    rootClass = supermod.GDSClassesMapping.get(tag)
    if rootClass is None and hasattr(supermod, tag):
        rootClass = getattr(supermod, tag)
    return tag, rootClass


def parse(inFilename):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'Annotation'
        rootClass = supermod.Annotation
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
##     sys.stdout.write('<?xml version="1.0" ?>\n')
##     rootObj.export(
##         sys.stdout, 0, name_=rootTag,
##         namespacedef_='',
##         pretty_print=True)
    return rootObj


def parseEtree(inFilename):
    doc = parsexml_(inFilename)
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
##     content = etree_.tostring(
##         rootElement, pretty_print=True,
##         xml_declaration=True, encoding="utf-8")
##     sys.stdout.write(content)
##     sys.stdout.write('\n')
    return rootObj, rootElement, mapping, reverse_mapping


def parseString(inString):
    from io import StringIO
    doc = parsexml_(StringIO(inString))
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'Annotation'
        rootClass = supermod.Annotation
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
##     sys.stdout.write('<?xml version="1.0" ?>\n')
##     rootObj.export(
##         sys.stdout, 0, name_=rootTag,
##         namespacedef_='')
    return rootObj


def parseLiteral(inFilename):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    roots = get_root_tag(rootNode)
    rootClass = roots[1]
    if rootClass is None:
        rootClass = supermod.Annotation
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
##     sys.stdout.write('#from ??? import *\n\n')
##     sys.stdout.write('import ??? as model_\n\n')
##     sys.stdout.write('rootObj = model_.Annotation(\n')
##     rootObj.exportLiteral(sys.stdout, 0, name_="Annotation")
##     sys.stdout.write(')\n')
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
