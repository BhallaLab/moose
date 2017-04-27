Shell
-----

.. py:class:: Shell


   .. py:method:: setclock

      (*destination message field*)      Assigns clock ticks. Args: tick#, dt


   .. py:method:: create

      (*destination message field*)      create( class, parent, newElm, name, numData, isGlobal )


   .. py:method:: delete

      (*destination message field*)      When applied to a regular object, this function operates on the Id (element) specified by the ObjId argument. The function deletes the entire object array on this Id, including all dataEntries on it,all its messages, and all its children. The DataIndex here is ignored, and all dataEntries are destroyed.
      When applied to a message: Destroys only that one specific message identified by the full ObjId.
      Args: ObjId



   .. py:method:: copy

      (*destination message field*)      handleCopy( vector< Id > args, string newName, unsigned int nCopies, bool toGlobal, bool copyExtMsgs ):  The vector< Id > has Id orig, Id newParent, Id newElm. This function copies an Element and all its children to a new parent. May also expand out the original into nCopies copies. Normally all messages within the copy tree are also copied.  If the flag copyExtMsgs is true, then all msgs going out are also copied.


   .. py:method:: move

      (*destination message field*)      handleMove( Id orig, Id newParent ): moves an Element to a new parent


   .. py:method:: addMsg

      (*destination message field*)      Makes a msg. Arguments are: msgtype, src object, src field, dest object, dest field


   .. py:method:: quit

      (*destination message field*)      Stops simulation running and quits the simulator


   .. py:method:: useClock

      (*destination message field*)      Deals with assignment of path to a given clock. Arguments: path, field, tick number.


.. py:class:: SimpleSynHandler

   The SimpleSynHandler handles simple synapses without plasticity. It uses a priority queue to manage them.

   .. py:method:: setNumSynapse

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumSynapse

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.
