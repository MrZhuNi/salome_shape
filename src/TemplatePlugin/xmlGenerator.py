'''
Created on Dec 18, 2014

@author: sbh
'''

import argparse
import os
import re

self_dir = os.path.dirname(os.path.realpath(__file__))
paths = {
    "FeaturesTemplate": os.path.abspath(os.path.join(self_dir, "features.xml.template")),
    "PluginsTemplate": os.path.abspath(os.path.join(self_dir, "../Config/", "plugins.xml.template")),
    "Features": os.path.abspath(os.path.join(self_dir, "plugin-Template_{0}.xml")),
    "Plugins": os.path.abspath(os.path.join(self_dir, "../Config/", "plugins.xml"))
}


def template(template_file_name):
    template_file = open(template_file_name)
    result = ""
    accumulate = False
    for line in template_file:
        if line.strip() == "<template>":
            accumulate = True
            continue
        elif line.strip() == "</template>":
            break
        if accumulate:
            result += line
    template_file.close()
    return result


def removeFeatureFiles():
    all_files = os.listdir(self_dir)
    reg_exp = re.compile(r"plugin-Template_\d+\.xml")
    all_files = [os.path.join(self_dir, x) for x in all_files if reg_exp.match(x)]
    for each_file in all_files:
        os.remove(each_file)


def generateFeaturesXml(plugin_number, features_count=1):
    features_template = template(paths["FeaturesTemplate"])
    features_template_file = open(paths["FeaturesTemplate"], "r")
    features_file_name = paths["Features"].format(plugin_number)
    features_file = open(features_file_name, "w")
    skip = False
    for line in features_template_file:
        if line.strip() == "<template>":
            skip = True
            for n in xrange(1, features_count + 1):
                features_file.write(features_template.format(nplugin=plugin_number, nfeature=n))
        elif line.strip() == "</template>":
            skip = False
            continue
        elif not skip:
            features_file.write(line)


def generatePluginsXml(plugins_count=1, features_count=1):
    plugins_template = template(paths["PluginsTemplate"])
    plugins_template_file = open(paths["PluginsTemplate"], "r")
    plugins_file = open(paths["Plugins"], "w")
    skip = False
    for line in plugins_template_file:
        if line.strip() == "<template>":
            skip = True
            for n in xrange(1, plugins_count + 1):
                plugins_file.write(plugins_template.format(nplugin=n))
                generateFeaturesXml(n, 5)
        elif line.strip() == "</template>":
            skip = False
            continue
        elif not skip:
            plugins_file.write(line)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--plugins', metavar='N', type=int, default=1)
    args = parser.parse_args()
    print "Number of plugins: ", args.plugins
    removeFeatureFiles()
    generatePluginsXml(args.plugins)
