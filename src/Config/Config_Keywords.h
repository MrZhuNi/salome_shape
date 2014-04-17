/*
 * Config_Keywords.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef CONFIG_KEYWORDS_H_
#define CONFIG_KEYWORDS_H_

/*
 * Hardcoded xml entities of plugin-*.xml
 */
const static char* NODE_WORKBENCH = "workbench";
const static char* NODE_GROUP = "group";
const static char* NODE_FEATURE = "feature";
//Widgets
const static char* WDG_DOUBLEVALUE = "doublevalue";
//Widget containers
const static char* WDG_GROUP = "groupbox";
const static char* WDG_CHECK_GROUP = "check_groupbox";
const static char* WDG_TOOLBOX = "toolbox";
const static char* WDG_TOOLBOX_BOX = "box";
const static char* WDG_SWITCH = "switch";
const static char* WDG_SWITCH_CASE = "case";


const static char* _ID = "id";
//const static char* WORKBENCH_ID = "id";
//const static char* GROUP_ID = "id";
//const static char* FEATURE_ID = "id";
const static char* FEATURE_TEXT = "text";
const static char* FEATURE_TOOLTIP = "tooltip";
const static char* FEATURE_ICON = "icon";
const static char* FEATURE_KEYSEQUENCE = "keysequence";

// doublevalue properties:
const static char* DOUBLE_WDG_MIN = "min";
const static char* DOUBLE_WDG_MAX = "max";
const static char* DOUBLE_WDG_STEP = "step";
const static char* DOUBLE_WDG_DFLT = "default";

//toolbox/switch properties
const static char* CONTAINER_PAGE_NAME = "title";


/*
 * Hardcoded xml entities of plugins.xml
 */
const static char* NODE_PLUGIN = "plugin";
const static char* NODE_PLUGINS = "plugins";

const static char* PLUGINS_MODULE = "module";
const static char* PLUGIN_CONFIG = "configuration";
const static char* PLUGIN_LIBRARY = "library";

#endif /* CONFIG_KEYWORDS_H_ */
