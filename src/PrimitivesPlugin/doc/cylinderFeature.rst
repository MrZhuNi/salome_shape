
Cylinder
========

The feature Cylinder creates a cylinder solid.

To create a Cylinder in the active part:

#. select in the Main Menu *Primitives - > Cylinder* item  or
#. click **Cylinder** button in the toolbar

.. image:: images/Cylinder_button.png
   :align: center

.. centered::
   **Cylinder** button 

There are 2 algorithms for creation of a Cylinder:

.. image:: images/cylinder_32x32.png
   :align: left
**Cylinder** creates a cylinder.

.. image:: images/cylinder_portion_32x32.png
   :align: left
**Portion of cylinder** creates a one segment of cylinder.

Cylinder
""""""""

.. image:: images/Cylinder.png
   :align: center
	
.. centered::
   **Cylinder**

The description of input fields for **Cylinder** tab is given below:

- selected **Point** defines center of the cylinder base;
- selected **Vector** defines the axis of the cylinder;
- Dimensions:      
   - **Radius**;
   - **Height**.    

**TUI Commands**:

* *model.addCylinder(Part_doc, Point, Axis, Radius, Height)*

**Arguments**:   1 Part + 1 vertex +1 vector +  2 values (dimensions: radius, height).

Portion of cylinder
"""""""""""""""""""

.. image:: images/Portion_cylinder.png
	   :align: center
		   
.. centered::
   **Portion of cylinder**  

Input fields for ***Portion of cylinder** tab includes in addition **Angle** to create a portion of cylinder.

**TUI Commands**:

* *model.addCylinder(Part_doc, Point, Axis, Radius, Height,Angle)*
  
**Arguments**:   1 Part + 1 vertex +1 vector +  3 values (dimensions: radius, height and angle).

Result
""""""

The Result of operation will be a SOLID.

Name is assigned automatically:
    
* **Cylinder_1**, **Cylinder_2**, ... to Feature;
* **Cylinder_1_1**, **Cylinder_2_1**, ... to Result.

Example is shown below.

.. image:: images/Cylinders.png
	   :align: center
		   
.. centered::
   Cylinders created  

**See Also** a sample TUI Script of a :ref:`tui_create_cylinder` operation.
