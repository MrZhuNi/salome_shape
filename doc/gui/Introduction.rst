
.. _introduction:


Introduction to SHAPER
======================

SHAPER module of SALOME is destined for:

- import and export of geometrical models in IGES, BREP, STEP and XAO formats;
- parametrical construction of geometrical objects using a wide range of functions;
    
  - creation of complex 2D sections using :ref:`sketchPlugin` with consequent extrusion or revolution :ref:`featuresPlugin`;
  - construction of model using primitives defined in  :ref:`primitivesPlugin`;
  - usage of Boolean operations :ref:`featuresPlugin`;
  
- viewing geometrical objects in the OCC viewer;
- transformation of geometrical objects using various algorithms.

It is possible to easily set :ref:`parameter_usage` predefined to be used as arguments when objects are created.

SHAPER module preferences are described in the SHAPER :ref:`preferences` section of SALOME Help.

Almost all SHAPER  module functionalities are accessible via Python Interface.

SHAPER module works with one study containing several documents:

- partset
- one or several parts.

Only one document can be active.
  
New study contains only Partset with 7 default constructions which cannot be deleted:
  
- one point **Origin** coincing with origin of coordinate system;
- three axes **OX**, **OY**, **OZ**  coinciding with coordinate axes;
- three planes **YOZ**, **XOZ**, **XOY**  coinciding with coordinate planes.    

Only  points, axis and plane (see  :ref:`constructionPlugin`) and sketches (see  :ref:`sketchPlugin`) can be added in Partset to be used in any part later.
:ref:`parameter_usage` can be used both in Partset and any Part.

     
New Part can be created as described in :ref:`partPlugin`.

Double click or pop-up menu can be used to activate existing document.


Application desktop
-------------------

The main window of the application consists of the following components:

- :ref:`main_menu`;
- :ref:`doc_windows`;
- :ref:`viewer`;
- :ref:`toolbar`;    
- status bar.    

.. image:: images/main_window.png
   :align: center

.. centered::
   Main window of SHAPER module

  
.. _main_menu:

Main menu 
---------

Main menu provides access to all commands of the application.

.. image:: images/main_menu.png
   :align: center

.. centered::
   Main menu

Main menu includes standard Salome items:

- File;
- Edit;
- View;
- Tools;   
- Window;
- Help;
    
and items specific for SHAPER module:

- Part (see :ref:`partPlugin`);
- Sketch (see :ref:`sketchPlugin`);
- Construction (see :ref:`constructionPlugin`);
- Build (see :ref:`buildPlugin`);
- Primitives (see :ref:`primitivesPlugin`);
- GDML (see :ref:`gdmlPlugin`);
- Features (see :ref:`featuresPlugin`);  
- Macros. 

 .. _toolbar:  

SHAPER toolbar
--------------

Application toolbar contains:

- standard toolbar;
- modukles toolbar;
- SHAPER toolbar.

.. image:: images/toolbar.png
   :align: center

.. centered::
   Toolbars
  
SHAPER toolbar duplicates main menu and  provides access to all commands of the application.

SHAPER toolbar is separated into sections:

- Part;
- Movement;
- Measurement;
- Sketch;
- Construction;
- Build;
- Primitives;
- GDML;
- Features;
- Boolean;
- Collections;    
- Fillet;  
- Macros.  

Visibility of SHAPER toolbar sections is managed using *View -> Toolbar* :ref:`main_menu` item or toolbar pop-up menu.
   
  
.. _doc_windows:

Dock windows
------------

Standard dock windows are: 

- :ref:`object_browser`
- :ref:`inspection_panel`
- :ref:`python console`
- :ref:`property_panel`

Dock windows can be placed in three dock areas using drag-and -drop:
  
- left, 
- right, 
- bottom.

By default object browser window is placed at the left dock area, Inspection panel at the right dock area and Python console at the bottom dock area of the main window.

Property Panel is hidden.

The Property Panel is shown on operation start at left dock area of the main window by default.

If Object Browser is shown at the same side then they will be tabbed.

Each dock window can be closed using **Cross** window button and opened again using a corresponding command from **View - Windows** :ref:`main_menu` or alternatively using pop-up menu.

.. image:: images/popup_menu.png
   :align: center

.. centered::
   Pop-up menu for visibilty of windows and toolbars


.. _object_browser: 

Object browser
^^^^^^^^^^^^^^

Object browser contains all documents created in the current study. 

Each document includes standard branches where created objects are placed.

Documents with branches are shown in Tree View.

.. image:: images/object_browser.png
   :align: center

.. centered::
   Object browser. Partset active


Partset includes three branches:

- Parameters;
- Constructions;
- Features    

Part includes the following branches:

- Parameters;
- Constructions;
- Results;
- Fields;
- Groups;   
- Features.    

New part contains only 3 empty branches:
 
- Parameters;
- Constructions;
- Results.

Features are placed one after another in the active document with default names and their results are placed into the corresponding branch.

Not defaullt branch is added into Tree View when the first feature of this type is created.

Note, that result used in feature as argument is removed from Tree View.

This child result can be restored using Recover Feature.

Each feature, result, construction, group, field, parameter can be renamed using *Rename* pop-up menu command.

.. image:: images/popup_menu_object_browser_feature.png
   :align: center

.. centered::
   Feature pop-up menu

.. image:: images/popup_menu_object_browser_result.png
   :align: center

.. centered::
    Result pop-up menu

.. image:: images/popup_menu_object_browser_construction.png
   :align: center

.. centered::
   Construction pop-up menu

Order of featurs can be changed using *Move to the end* pop-up menu command. Selected feature will be moved to the end of feture list.

Folders can be used to arrange long Tree View for features.

.. image:: images/object_browser_folder.png
   :align: center

.. centered::
   Object browser with folder Dome. Part_1 active

*Insert a folder before* pop-up menu command creates a new empty folder before the selected feature. Folder can be renamed.

Features placed after/before the created folder can be moved into the folder using *Move into the previous folder* / *Move into the next folder* pop-up menu command.

This allows to decrease length of feature list.

Features can be removed from folder back using *Move out before the folder* / *Move out after the folder* pop-up menu command.

*Clean history* pop-up menu command looks for features that do not participate in other features.

**Clean history** dialog box shows list of unused features. After confirmation by click **Yes** button unused features are removed.

.. image:: images/clean_history.png
   :align: center

.. centered::
   **Clean history** dialog box


*Select result* and *Select parent feature* pop-up menu commands help to navigate along Tree View.


Feature together with its result can be deleted using *Delete* pop-up menu command.

Note that all features using removed feature and their results will be removed simulteneously.

**Delete feature** dialog box shows list of feature to be removed. After confirmation by click **Yes** button all these features are removed.


.. image:: images/delete_feature.png
   :align: center

.. centered::
   **Delete feature** dialog box
   
.. _property_panel:

Property panel
^^^^^^^^^^^^^^

By default Property Panel is hidden.

The Property Panel is shown on operation start at left side of the main window by default.

If Object Browser is shown at the same side then it will be tabbed with Property Panel.

Property panel consists of two parts:

- controls container for input of parameters for the current operation;
- buttons panel containing standard buttons:
    
  - **Ok/Apply** executes operation with defined parameters,
  - **Cancel/Close** calls **Abort operation** dialog box to confirm  operation abort,
  - **Help** calls User's guide opened on page describing the current operation.

.. image:: images/button_ok.png
   :align: center

.. centered::
   **OK/Apply**  button

.. image:: images/button_cancel.png
   :align: center

.. centered::
   **Cancel/Close**  button

.. image:: images/button_help.png
   :align: center

.. centered::
   **Help**  button

.. image:: images/abort_operation.png
   :align: center

.. centered::
   **Abort operation** dialog box


**OK/Apply**  button is disabled if not all input parameters are defined or some errors are found. Error is shown as tooltip and in status bar.

.. _inspection_panel: 

Inspection panel 
^^^^^^^^^^^^^^^^

Inspection panel provides the list of types and quantities of all topological entities, composing the selected result, consruction  or feature.

.. image:: images/inspection_panel.png
   :align: center

.. centered::
   Inspection panel for default Box

**Object** displays name of the selected result, consruction  or feature.

**Type** characterizes the whole shape.
   
The information about Point, Axis or Edge  additionally shows coordinates of point / end points.

.. image:: images/inspection_panel_line.png
   :align: center

.. centered::
   Inspection panel for Axis
   

The information about  Plane, Face additionally shows coordinates of center point and direction of normal.

.. image:: images/inspection_panel_line.png
   :align: center

.. centered::
   Inspection panel for Face 

 

.. _python console:

Python console
^^^^^^^^^^^^^^

Python console interpreters Python commands entered manually.

In particular, it is possble to load python script:

*execfile(r"/dn48/newgeom/data/example.py")*

Pop-up menu allows:

- Copy selected text to clipboard;
- Paste text from clipboard to Python console ;
- Clear Python console;
- Dump commands from console into the specified file;
- Start/Stop writing log into the specified file.    
 

.. image:: images/python_console_popup.png
   :align: center

.. centered::
   Pop-up menu of Python console

.. _viewer:

Viewer
------

The application supports one OCC 3D viewer and is able to show only one 3D space.

This 3D space can be represented in several view windows. New view window can be created using **Clone view** button in viewer toolbar.

Each of view windows represents its own point of view on the 3D scene.

This point of view can be modified by user with help of viewer commands like **Panning**, **Zooming**, **Scaling** and so on.


.. image:: images/2_viewers.png
   :align: center

.. centered::
   Two view windows

Description of OCC 3D Viewer architecture and functionality is provided in GUI module user's guide in chapter **OCC 3D Viewer**.


.. _parameter_usage:

Parameter usage
---------------

Model parametrization can be done using parameters.

Parameter can be created in the active partset or part by:

- :ref:`parameter`;
- :ref:`parameters`;
- :ref:`parameter_expression`.

Any argument in features can be defined as parameter or expression containing parameters.

List of features using parameters is given in **Parameters** dialog box:

 .. image:: images/parameters_feature.png
   :align: center

.. centered::
   Parameters dialog box
  
If parameter value is changed, then all features where it is used are rebuilt.

Parameter name should be unique in the active document.

However, partset and part can have parameters with the same name. If parameter name in Partset and Part are identical, then Part parameter has higher prority and its value will be used in the features of this part.

In contrast to features (see :ref:`object_browser`), there is an additional option  when Delete parameters.   

.. image:: images/delete_parameter.png
   :align: center

.. centered::
   Delete parameter

After click **Replace** button. selected parameter is removed but parent parameters and features are not removed. Deleted parameter is replaced by its value.

.. _parameter_expression:

Create parameter on fly
^^^^^^^^^^^^^^^^^^^^^^^

Parameter can be created during feature creation simply writing *variable=expression* in any editbox.

After validation of feature a new parameter with given name **variable** and value equal to evaluated expression appears in object browser under **Parameters** in the active partset or part.

.. _preferences:

SHAPER preferences
------------------

Description of General application preferences and **Preferences** dialog box is provided in GUI module user's guide in chapter **Setting Preferences**.

SHAPER preferences define visualization of objects, visualization during selection, edition. New preferences can be used right after modification or later after activation of SHAPER module.

To call **Preferences** dialog box:

#. select in the Main Menu *File - > Preferences* item or
#. use  **Ctrl+P** shortcut.

SHAPER preferences include 4 tabs:

- :ref:`visualization_preferences`;
- :ref:`plugins_preferences`;
- :ref:`sketch_preferences`;
- :ref:`viewer_preferences`.

Visualization tab is activated by default when **Preferences** dialog box is opened in active SHAPER module.

Other tabs are activated by clck on tab header.
  
.. _visualization_preferences:

Visualization tab
^^^^^^^^^^^^^^^^^

This tab defines presentation of objects displayed in OCC 3D viewer.

.. image:: images/visualization_preferences.png
   :align: center

.. centered::
   Preferences - Visualization tab

**Input fields**:

- **Result color** selects default shading color for objects from **Results** branch;
- **Group color** selects default color for objects from **Group** branch;
- **Construction color** selects default color for objects from **Constructions** branch;
- **Part color** selects default color for parts shown in Partset;  
- **Field color** selects default color for objects from **Field** branch;
- **Body deflection coefficient** defines default deflection coefficient for objects from **Results** branch. A smaller coefficient provides better quality of a shape in the viewer;
- **Construction deflection coefficient** defines default deflection coefficient for objects from **Construction** branch. A smaller coefficient provides better quality of a shape in the viewer;
- **Reference shape wireframe color in operation** selects default color used for wireframe visualization of objects used in active operation;
- **Result shape wireframe color in operation** selects default color used for wireframe visualization of result in active operation. Click **See preview** button to show result;
- **Multi selector item color in operation** selects default color used for wireframe visualization of objects selected in propertry panel to distiguish them among all objects used in active operation;
- **Color of removed feature in operation** selects default color used for visualization of sketch entities to be removed during  Trim/Split operations;
- **Color of sketch plane** selects default shading color for objects from **Results** branch;
- **Hidden faces transparency** defines default transparency value for hidden faces;
- **Dimension arrow size**  defines default size of arrows for extension line showing dimensional constraint;  
- **Dimension font** defines font used for value of dimensional constraint;
- **Dimension value size**  defines default size of value for dimensional constraint;
- **Sketch dimension color**  defines default color of dimensional constraint; 
- **Construction plane color** selects default color for Construction planes;  
- **Sketch entity color** selects default color for sketch objects;
- **Sketch external entity color** selects default color for external objects selected as reference during sketch creation/edition;
- **Sketch auxiliary entity color** selects default color for sketch auxiliary objects;
- **Sketch overconsrtaint color** selects default color for sketch with redundant constraints;
- **Sketch fully consrtaint color** selects default color for sketch with zero degrees of freedom.
  
To redefine any color click on the corrersponding line to acccsess **Select color** dialog box

.. image:: images/select_color.png
   :align: center

.. centered::
   **Select color** dialog box
   
Preferences for sketch are applicable  during sketch creation/edition operation.
   
.. _plugins_preferences:
   
Plugins tab
^^^^^^^^^^^
Plugins tab defines folders where plugins and resources are located.

.. image:: images/plugins_preferences.png
   :align: center

.. centered::
   Preferences - Plugins tab

**Input fields**:

- **Default path** selects default folder where plugins are located. Click on **Open** button opens standard **Find directory** dialog box to navigate to desired folder;

- **Import initial directory** selects default folder where resources are located. Click on **Open** button opens standard **Find directory** dialog box to navigate to desired folder.

.. image:: images/open_button.png
   :align: center

.. centered::
   **Open** button

.. image:: images/find_directory.png
   :align: center

.. centered::
   **Find directory** dialog box
    
   
.. _sketch_preferences:
   
Sketch tab
^^^^^^^^^^

Sketch tab defines properties of coordinate planes shown for selection of sketch plane when no convinient objects are shown in OCC 3D viewer.

.. image:: images/sketch_preferences.png
   :align: center

.. centered::
   Preferences - Sketch tab

**Input fields**:

- **Size** defines size of coordinate planes;
- **Thickness**  defines thickness of coordinate plane borders; 
- **Rotate to plane when selected** check-box turns on/off automatic switch the viewer to the top view for the selected sketch plane.  

   
.. _viewer_preferences:
   
Viewer tab
^^^^^^^^^^

Viewer tab defines selection in OCC 3D viewer properties. 

.. image:: images/viewer_preferences.png
   :align: center

.. centered::
   Preferences - Viewer tab   

**Input fields**:

- **Default Selection** defines objects to be selected by mouse click in OCC 3D viewer:

  - **Faces** check-box turns on/off selection of faces;
  - **Edges** check-box turns on/off selection of edges;
  - **Vertices** check-box turns on/off selection of vertices;

- **Selection sensitivity** defines size of area around object in pixels, in which  mouse click selects object inside this area:

  - **Vertex** defines selection  sensitivity for vertices; 
  - **Edge**  defines selection  sensitivity for edges.  
