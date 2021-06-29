.. _pipeNetwork:
.. |pipeNetwork.icon|    image:: images/pipeNetwork.png

Réseau de tuyaux
================

Cette macro crée les objets SHAPER correspondant à un réseau de tuyauteries décrit dans un fichier texte.

Pour créer l'objet de la tuyauterie :

#. Choisir dans le menu principal *Macros - > Réseau de tuyaux* ou
#. Cliquer le bouton |pipeNetwork.icon| **Réseau de tuyaux** dans la barre des macros.

Le menu suivant apparaît :

.. figure:: images/pipeNetworkPanel.png
   :align: center

   Réseau de tuyaux

On doit fournir le fichier de type texte qui contient le réseau. Sa syntaxe est décrite plus bas.

Syntaxe du fichier de données
"""""""""""""""""""""""""""""

Règles générales
----------------

- Chaque ligne commençant par # est ignorée
- Une information par ligne
- Sur une ligne, les données sont séparés par des blancs
- Les données sont regroupées en 3 sections : les noeuds, la connectivité, les raccordements
- Une section débute par **mot-clé section** est finit par *............*

Les noeuds
----------

- Repérage avec **nodes section**

- Pour un noeud dont on donne les coordonnées en absolu :

``Identifiant du noeud, le signe -, les 3 coordonnées``

- Pour un noeud défini par rapport à un autre :

``Identifiant du noeud, identifiant du noeud de départ, les 3 coordonnées de la translation``

La connectivité
---------------

- Repérage avec **connectivity section**
- Choix de la méthode de description avec **method=par_ligne** ou **method=2par2**

- Pour la méthode en ligne :

Chaque ligne est décrite par la suite de ses noeuds repérés par leurs identifiants :

``Identifiant du noeud n°1, identifiant du noeud n°2, ..., identifiant du dernier noeud``

- Pour la méthode 2 par 2 :

Chaque tronçon est décrit par les 2 noeuds repérés par leurs identifiants :

``Identifiant du noeud n°1 , identifiant du noeud n°2``

Exemple
-------
.. literalinclude:: ../../../../../../bin/salome/macros/pipeNetwork/pipeNetwork_ligne.txt
