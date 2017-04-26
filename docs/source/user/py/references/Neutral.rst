Neutral
-------

.. py:class:: Neutral

   Neutral: Base class for all MOOSE classes. Providesaccess functions for housekeeping fields and operations, messagetraversal, and so on.

   .. py:method:: parentMsg

      (*destination message field*)      Message from Parent Element(s)


   .. py:method:: setThis

      (*destination message field*)      Assigns field value.


   .. py:method:: getThis

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setName

      (*destination message field*)      Assigns field value.


   .. py:method:: getName

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMe

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getParent

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getChildren

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getPath

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getClassName

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumData

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumData

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumField

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumField

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIdValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIndex

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getFieldIndex

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTick

      (*destination message field*)      Assigns field value.


   .. py:method:: getTick

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getValueFields

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSourceFields

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDestFields

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMsgOut

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMsgIn

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNeighbors

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMsgDests

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMsgDestFunctions

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIsA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: childOut

      int (*source message field*)      Message to child Elements


   .. py:attribute:: this

      Neutral (*value field*)      Access function for entire object


   .. py:attribute:: name

      string (*value field*)      Name of object


   .. py:attribute:: me

      ObjId (*value field*)      ObjId for current object


   .. py:attribute:: parent

      ObjId (*value field*)      Parent ObjId for current object


   .. py:attribute:: children

      vector<Id> (*value field*)      vector of ObjIds listing all children of current object


   .. py:attribute:: path

      string (*value field*)      text path for object


   .. py:attribute:: className

      string (*value field*)      Class Name of object


   .. py:attribute:: numData

      unsigned int (*value field*)      # of Data entries on Element.Note that on a FieldElement this does NOT refer to field entries,but to the number of DataEntries on the parent of the FieldElement.


   .. py:attribute:: numField

      unsigned int (*value field*)      For a FieldElement: number of entries of self.For a regular Element: One.


   .. py:attribute:: idValue

      unsigned int (*value field*)      Object id of self, converted to an unsigned int.


   .. py:attribute:: index

      unsigned int (*value field*)      For a FieldElement: Object index of parent.For a regular Element: Object index (dataId) of self.


   .. py:attribute:: fieldIndex

      unsigned int (*value field*)      For a FieldElement: field Index of self.For a regular Element: zero.


   .. py:attribute:: tick

      int (*value field*)      Clock tick for this Element for periodic execution in the main simulation event loop. A default is normally assigned, based on object class, but one can override to any value between 0 and 19. Assigning to -1 means that the object is disabled and will not be called during simulation execution The actual timestep (dt) belonging to a clock tick is defined by the Clock object.


   .. py:attribute:: dt

      double (*value field*)      Timestep used for this Element. Zero if not scheduled.


   .. py:attribute:: valueFields

      vector<string> (*value field*)      List of all value fields on Element.These fields are accessed through the assignment operations in the Python interface.These fields may also be accessed as functions through the set<FieldName> and get<FieldName> commands.


   .. py:attribute:: sourceFields

      vector<string> (*value field*)      List of all source fields on Element, that is fields that can act as message sources.


   .. py:attribute:: destFields

      vector<string> (*value field*)      List of all destination fields on Element, that is, fieldsthat are accessed as Element functions.


   .. py:attribute:: msgOut

      vector<ObjId> (*value field*)      Messages going out from this Element


   .. py:attribute:: msgIn

      vector<ObjId> (*value field*)      Messages coming in to this Element


   .. py:attribute:: neighbors

      string,vector<Id> (*lookup field*)      Ids of Elements connected this Element on specified field.


   .. py:attribute:: msgDests

      string,vector<ObjId> (*lookup field*)      ObjIds receiving messages from the specified SrcFinfo


   .. py:attribute:: msgDestFunctions

      string,vector<string> (*lookup field*)      Matching function names for each ObjId receiving a msg from the specified SrcFinfo


   .. py:attribute:: isA

      string,bool (*lookup field*)      Returns true if the current object is derived from the specified the specified class
