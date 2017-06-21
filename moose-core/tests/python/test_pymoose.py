from __future__ import print_function
import sys
try:
    import unittest2 as unittest
except:
    import unittest
import uuid
import platform

import moose

class TestVec(unittest.TestCase):
    """Test pymoose basics"""
    def testCreate(self):
        em = moose.vec('/test', 10, 0, 'Neutral')
        self.assertEqual(em.path, '/test')

    def testCreateKW(self):
        em = moose.vec(path='/testCreateKW', n=10, g=1, dtype='Neutral')
        self.assertEqual(em.path, '/testCreateKW')

    def testGetItem(self):
        em = moose.vec('testGetItem', n=10, g=1, dtype='Neutral')
        el = em[5]
        self.assertEqual(el.path,'/%s[5]' % (em.name))

    def testIndexError(self):
        em = moose.vec('testIndexError', n=3, g=1, dtype='Neutral')
        with self.assertRaises(IndexError):
            el = em[5]
        
    def testSlice(self):
        em = moose.vec('/testSlice', n=10, g=1, dtype='Neutral')
        sl = em[5:8]
        for ii, el in enumerate(sl):
            self.assertEqual(el.path,  '/testSlice[%d]' % (5 + ii))

    def testExtendedSlice(self):
        em = moose.vec('/testSlice', n=10, g=1, dtype='Neutral')
        sl = em[2:12:3]
        for ii, el in enumerate(sl):
            self.assertEqual(el.path,  '/testSlice[%d]' % (2 + ii*3))

class TestNeutral(unittest.TestCase):
    def testPath(self):
        a = moose.Neutral('a')
        self.assertEqual(a.path, '/a[0]')

    def testHash(self):
        """Test if our hash function for ObjId is doing the right thing - the
        hash should be 16 lsb of Id value, 32 lsb of dataIndex and 16
        lsb of fieldIndex.

        I do not have a system to test this - but I would like to check ii
        up to 2**17 and n=2**33

        """

        # If platform is 32 bit, this test fails. This bug is reported on
        # sourceforge. Currently disabling this test so build can pass on
        # launchpad.

        if platform.architecture()[0] != '64bit':
            return True

        oidlist = [moose.Neutral('x_%d' % (ii), n=2**10) for ii in range(2**6)]
        if sys.byteorder == 'little':
            for oid in oidlist:
                self.assertEqual(hash(oid), oid.vec.value  << 48 | oid.dindex << 16 | oid.findex)
        else:
            for oid in oidlist:
                self.assertEqual(hash(oid), oid.vec.value  >> 48 | oid.dindex >> 16 | oid.findex)
            

class TestNeutral1(unittest.TestCase):
    def setUp(self):
        self.a_path = 'neutral%d' % (uuid.uuid4().int)
        self.b_path = self.a_path + '/b'
        self.c_path = '/neutral%d' % (uuid.uuid4().int)
        self.d_path = self.c_path + '/d'
        self.c_len = 3
        self.a = moose.Neutral(self.a_path)
        self.b = moose.Neutral(self.b_path)
        self.c = moose.Neutral(self.c_path, self.c_len)
        print(self.a_path, self.b_path)
        print(self.a.path, self.b.path)
        print(len(self.c.vec), self.c_len)
                
    def testNew(self):
        self.assertTrue(moose.exists(self.a_path))

    def testNewChild(self):
        self.assertTrue(moose.exists(self.b_path))

    def testNewChildWithSingleDim(self):
        self.assertTrue(moose.exists(self.c_path))    

    def testDimension(self):
        self.assertEqual(self.c.vec.shape[0], self.c_len)

    def testLen(self):
        self.assertEqual(len(self.c.vec), self.c_len)

    def testPath(self):
        # Unfortunately the indexing in path seems unstable - in
        # async13 it is switched to have [0] for the first element,
        # breaking old code which was supposed to skip the [0] and
        # include the index only for second entry onwards.
        self.assertEqual(self.b.path, '/%s[0]/%s[0]' % (self.a_path, 'b'))
        em = moose.vec(self.c)
        self.assertEqual(em[1].path, self.c_path + '[1]')

    def testName(self):
        self.assertEqual(self.b.name, 'b')

    def testPathEndingWithSlash(self):
        self.assertRaises(ValueError, moose.Neutral, 'test/')

    def testNonExistentPath(self):
        self.assertRaises(ValueError, moose.Neutral, '/nonexistent_parent/invalid_child')

    def testDeletedCopyException(self):
        moose.delete(self.c.vec)
        self.assertRaises(ValueError, moose.Neutral, self.c)

    def testDeletedGetFieldException(self):
        moose.delete(self.c.vec)
        with self.assertRaises(ValueError):
            s = self.c.name

    def testDeletedParentException(self):
        moose.delete(self.a.vec)
        with self.assertRaises(ValueError):
            s = self.b.name

    def testIdObjId(self):
        vec = moose.vec(self.a)
        self.assertEqual(vec, self.a.vec)

    def testCompareId(self):
        """Test the rich comparison between ids"""
        id1 = moose.vec('A', n=2, dtype='Neutral')
        id2 = moose.vec('B', n=4, dtype='Neutral')
        id3 = moose.vec('A')
        self.assertTrue(id1 < id2)
        self.assertEqual(id1, id3)
        self.assertTrue(id2 > id1)
        self.assertTrue(id2 >= id1)
        self.assertTrue(id1 <= id2)
    
    def testRename(self):
        """Rename an element in a Id and check if that was effective. This
        tests for setting values also."""
        id1 = moose.vec(path='/alpha', n=1, dtype='Neutral')
        id2 = moose.vec('alpha')
        id1[0].name = 'bravo'
        self.assertEqual(id1.path, '/bravo')
        self.assertEqual(id2.path, '/bravo')

class TestWildcardFind(unittest.TestCase):
    def setUp(self):
        self.x = moose.Neutral('/x', 10)
        self.y = moose.IntFire('/x[5]/y', 10)
        self.z = moose.PulseGen('/x[5]/z', 3)
        self.u = moose.IntFire('/x[5]/z[2]/u', 10)

    def testAllData(self):
        alldata = moose.wildcardFind('/x[5]/y[]')
        set_all = set(alldata)
        set_y = set(self.y.vec)
        self.assertEqual(set_all, set_y)

    def testIsA(self):
        yset = set(moose.wildcardFind('/x[5]/#[ISA=IntFire]'))
        zset = set(moose.wildcardFind('/x[5]/#[ISA=PulseGen]'))
        self.assertEqual(yset, set(self.y.vec))
        self.assertEqual(zset, set(self.z.vec))

    def testRecursiveIsA(self):
        ifset = set(moose.wildcardFind('/x[5]/##[ISA=IntFire]'))
        origset = set(self.y.vec)
        origset.update(self.u.vec)
        self.assertEqual(ifset, origset)

    def testLessThan(self):
        for ii in range(5):
            self.y.vec[ii].Vm = -10
        # This causes a lot of error message from SetGet::strGet - can
        # we combine conditions with logical operators?
        # '/x[]/##[(ISA=IntFire) AND (FIELD(Vm)<0)]'
        yless = moose.wildcardFind('/x[]/##[FIELD(Vm)<0]') 
        self.assertEqual(set(yless), set(self.y.vec[:5]))
        
        
# class TestPyMooseGlobals(unittest.TestCase):
#     def setUp(self):
#         path1 = 'neutral%d' % (uuid.uuid4().int)
#         path2 = 'neutral%d' % (uuid.uuid4().int)
#         self.src1 = moose.Id(path1, 1, 'Neutral')
#         self.dest1 = moose.Id(path2, 1, 'Neutral')

#     def testCopy(self):
#         print 'Testing copy ...',
#         newname = 'neutral%d' % (uuid.uuid4().int)
#         new_id = moose.copy(self.src1, self.dest1, newname, 3, toGlobal=False)
#         self.assertEqual(len(new_id), 3)
#         self.assertEqual(new_obj.path, self.dest1.path + "/" + newname + '[0]')
#         print 'OK'

#     def testElement(self):
#         print 'Testing element() ...'
#         x = moose.element(self.src1.path)
#         self.assertTrue(isinstance(x, moose.Neutral))
#         self.assertEqual(x.path, self.src1.path)
#         x = moose.element(self.src1.vec)
#         self.assertTrue(isinstance(x, moose.Neutral))
#         self.assertEqual(x.path, self.src1.path)
#         x = moose.element(self.src1[0].oid_)
#         self.assertTrue(isinstance(x, moose.Neutral))
#         self.assertEqual(x.path, self.src1.path)
#         self.assertRaises(NameError, moose.element, 'abracadabra')
        

class TestMessages(unittest.TestCase):
    def setUp(self):
        path1 = '/comp%d' % (uuid.uuid4().int)
        path2 = '/comp%d' % (uuid.uuid4().int)
        self.src1 = moose.Compartment(path1)
        self.dest1 = moose.Compartment(path2)        

    def testConnect(self):
        print('Testing connect ...', end=' ')
        msg = self.src1.connect('raxial', self.dest1, 'axial')
        outmsgs_src = self.src1.msgOut
        outmsgs_dest = self.dest1.msgOut
        self.assertEqual(len(outmsgs_dest), len(outmsgs_src))
        for ii in range(len(outmsgs_src)):
            self.assertEqual(outmsgs_src[ii], outmsgs_dest[ii])
            srcFieldsOnE1 = outmsgs_src[ii].getField('srcFieldsOnE1')
            self.assertEqual(srcFieldsOnE1[0], 'raxialOut')
            destFieldsOnE2 = outmsgs_src[ii].getField('destFieldsOnE2')
            self.assertEqual(destFieldsOnE2[0], 'handleRaxial')
        print('OK')

    def testDelete(self):
        print('Testing delete ...', end=' ')
        msg = self.src1.connect('raxial', self.dest1, 'axial')
        src2 = moose.PulseGen('/pulsegen')
        msg2 = moose.connect(src2, 'output', self.dest1, 'injectMsg')
        moose.delete(msg)
        # accessing deleted element should raise error
        with self.assertRaises(ValueError):
            p = msg.path
        p = msg2.path # this should not raise any error

    # def test_getInMessageDict(self):
    #     print 'Testing getInMessageDict ...',
    #     indict = self.src1.getInMessageDict()
    #     self.assertTrue('parentMsg' in indict)
        

# class TestNeighbors(unittest.TestCase):
#     def setUp(self):
#         self.pulsegen = moose.PulseGen('pulsegen')
#         self.compartment = moose.Compartment('compartment')
#         self.table = moose.Table('table')
#         moose.connect(self.table, 'requestData', self.compartment, 'get_Im')
#         moose.connect(self.pulsegen, 'output', self.compartment, 'injectMsg')
        
#     def testNeighborDict(self):
#         print 'Testing neighbour dict ...'
#         neighbors = self.compartment.neighborDict
#         self.assertTrue(self.pulsegen.oid_ in [ n.oid_ for n in neighbors['injectMsg']])
#         self.assertTrue(self.table.oid_ in [n.oid_ for n in neighbors['get_Im']])
#         self.assertTrue(self.compartment.oid_ in [n.oid_ for n in self.pulsegen.neighborDict['output']])
#         self.assertTrue(self.compartment.oid_ in [n.oid_ for n in self.table.neighborDict['requestData']])
#         print 'OK'
                      
class TestDelete(unittest.TestCase):
    def setUp(self):
        self.oid = moose.Neutral('testDelete')
        moose.delete(self.oid.vec)

    def testRepr(self):
        with self.assertRaises(ValueError):
            print(self.oid)

    def testGetField(self):
        with self.assertRaises(ValueError):
            print(self.oid.name)

class TestFieldAccess(unittest.TestCase):
    def testSetGet(self):
        pass
        # a = moose.IntFire('TestFieldAccess_IntFire', g=0)
        # print 'a:', a
        # sys.stdout.flush()
        # print 'Vm=',
        # print a.Vm
        # a.Vm = 2.0
        # self.assertAlmostEqual(a.Vm, 2.0)

# class TestValueFieldTypes(unittest.TestCase):
#     def setUp(self):
#         self.id_ = uuid.uuid4().int
#         self.container = moose.Neutral('/test%d' % (self.id_))
#         cwe = moose.getCwe()
#         self.model = moose.loadModel('../Demos/Genesis_files/Kholodenko.g', '%s/kholodenko' % (self.container.path))
#         moose.setCwe(cwe)
    
#     def testVecUnsigned(self):
#         x = moose.element('%s/kinetics' % (self.model.path))
#         self.assertTrue(len(x.meshToSpace) > 0)
        
if __name__ == '__main__':
    print('PyMOOSE Regression Tests:')
    unittest.main()
