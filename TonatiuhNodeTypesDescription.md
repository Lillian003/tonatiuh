# Tonatiuh Scene #

A scene consists of one or more nodes hierarchically oredered.  To create scenes, the nodes are added as children of grouping nodes.

<a href='https://picasaweb.google.com/lh/photo/jtG7cObFSFi7fZZf_8LSW-hPzBptKnN6WXkW8rDyw9U?feat=directlink'><img src='https://lh4.googleusercontent.com/-CuUiZ8vAoKw/ULXFSnjAAiI/AAAAAAAAAL4/duY64hNAazU/s288/TreeView.png' />


<h1>Type of Nodes</h1>

A node is each of the elements of the scene containing the data and methods to define the geometries, properties, ... of the scene.<br>
<br>
<h2>Group node <img src='http://tonatiuh.googlecode.com/svn/wiki/images/userguide/separatorKit.png' /></h2>
Is a node that groups other nodes of the scene. This type of node can group nodes of the same type or TShapeKit type nodes.<br>
<br>
The user can define the following paramenters to this group node:<br>
<br>
<ul><li>translation: the translation in x, y, and z. The default value is (0.0 0.0 0.0).<br>
</li><li>rotation: the rotation in terms of an axis and an angle. The default value is (0.0 0.0 1.0), 0.0.<br>
</li><li>scaleFactor: the scaling factor in x, y, and z. The default value is (1.0 1.0 1.0).<br>
</li><li>scaleOrientation: the rotation to apply before the scale is applied. The default value is (0.0 0.0 1.0), 0.0.<br>
</li><li>center: the center point for rotation and scaling. The default value is (0.0 0.0 0.0).</li></ul>

The parameters are applied so that the the center parameter affects first,followed by the scale orientation, the scaling factor, the rotation, and the translation.<br>
<br>
<h2>Surface node <img src='http://tonatiuh.googlecode.com/svn/wiki/images/userguide/shapeKit.png' /></h2>
Is also a group node. It croups the nodes that define a surface properties. Surface nodes can contain a shape node and a material node.<br>
<br>
<br>
<h2>Shape node</h2>
Is a node that defines a surface geometry. Node parameters depend on the selected shape node type.<br>
<br>
Shape nodes always are leaf nodes in the scene.<br>
<br>
Now, we can use this shapes into Tonatiuh:<br>
<ul><li>Bezier Surface<br>
</li><li>Cone<br>
</li><li>Cylinder<br>
</li><li>Flat disk<br>
</li><li>Flat rectangle<br>
</li><li>Flat triangle<br>
</li><li>Hyperboloid<br>
</li><li>Parabolic dish<br>
</li><li>Parabolic rectangle<br>
</li><li>Sphere<br>
</li><li>Spherical polygon<br>
</li><li>Spherical rectangle<br>
</li><li><a href='ShapeTroughAsymmetricCPC.md'>Trough asymmetric CPC</a>
</li><li>Trough CHC<br>
</li><li>Trough CPC<br>
</li><li>Trough hyperbola<br>
</li><li>Trough Parabola<br>
</li><li><a href='ShapeTrumpet.md'>Trumpet</a></li></ul>


<h2>Material node</h2>

Material nodes defines surfaces propersties and appearance characteristics as ambient color, diffuse color, specular color, emissive color, shininess, and transparency.<br>
<br>
Material nodes always are leaf nodes in the scene.<br>
<br>
<h2>Tracker node</h2>
Tracker nodes are <i>Group nodes</i> children. In a <i>Group node</i> only can be one tracker node child.<br>
<br>
These nodes change the parent node parameters automatically when a scene light position changes. If you define a TSeparatorKit node parameter value manually and this node has a tracker child, when the light position changes this value will be removed.<br>
<br>
<a href='UsersGuide.md'>User's Guide</a> | <a href='http://code.google.com/p/tonatiuh/w/list'>Wiki Home</a>