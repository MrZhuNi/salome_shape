
Distance constraint
===================

Distance can be defined between two objects such as point, line, line or arc end point, center of circle or arc.

To create Distance constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Distance* item  or
#. click **Distance** button in Sketch toolbar:

.. image:: images/distance.png
   :align: center

.. centered::
   **Distance**  button

Property panel:

.. image:: images/Distance_panel.png
   :align: center

Input fields:

- **First object** - the first object
- **Second object** - the second object
- **Value** - distance between the objects, could be modified to set the desirable value
- **Text location** - position of the distance value label relating to extension line (in the view)
   .. image:: images/location_left.png
      :align: left
   **Left** inserts text at the left of the distance extension line.

   .. image:: images/location_automatic.png
      :align: left
   **Automatic** inserts text at the middle of the distance extension line if it has enough length, otherwise - to the left.

   .. image:: images/location_right.png
      :align: left
   **Right** inserts text to the right of the distance extension line.
- **Keep orientation** - fixes the relative position between selected objects

When the both objects are selected distance value is displayed in the property panel and in the view.

When creating the constraint and the both objects are selected the first time:

- drag the distance presentation in the view to the desired position (by move mouse and click once)
- set desirable distance value in the input field in the view and press **Enter** or just press **Enter** to keep the current distance

.. image:: images/Distance_field_view.png
   :align: center

.. centered::
   Distance input in the view

**TUI Command**: *Sketch_1.setDistance(FirstObject, SecondObject, Value, KeepOrientation)*

**Arguments**:  2 objects + distance value + keep orientation flag

Result
""""""

Created Distance appears in the view.

.. image:: images/Distance_res.png
	   :align: center

.. centered::
   Distance created

**See Also** a sample TUI Script of a :ref:`tui_create_distance` operation.
