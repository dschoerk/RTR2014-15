# Demo: Disco Fever

__DoDa Productions__

_VU Echtzeitgraphik WS2014 3.0, 186.140_

Dominik Schörkhuber & David Pfahler

## Kurzbeschreibung

Hellknight und Imp Models (Doom3) werden durch den zentralen Gang der Crytek Sponza bewegt. Am Ziel angekommen beginnen sie zu Gangnam Style zu tanzen. Während die Musik startet wird die Belichtung der Szene verändert. Zu Beginn erhellt 
strahlender Sonnenschein die Szene, danach wird die Szene von mehreren Punktlichtern im Diskoambiente ausgeleuchtet.

## Effekte

### Deferred Rendering [1,2,3]

Durch das Deferred Shading wollen wir die Szene mit mehreren dynamischen Lichtern beleuchtet werden. 
Zur Beleuchtung verwenden wir das Blinn Phong Beleuchtungsmodell

### Bump Mapping [4]

Auf allen Oberflächen 

### Variance Shadow Maps - Softshadows [11,12]

Auf allen Lichtquellen

### Skeleton Animation [7]

Für die Tanzbewegungen der Monster

### Depth of Field - Postprocessing [8]

Abhängig vom Kamera Focus werden Bildteile blurry dargestellt

### SSAO - Postprocessing [9]

Screenspace Ambient Occulsion um Verdeckungen naher Objekte zu simulieren 

### Bloom - Postprocessing [10]

Soll einen HDR Effekt simulieren und Lichtquellen durch Bloom darstellen

## Erweiterung

Falls noch Zeit ist

### Shadow Volumes - Hardshadows [5,6]
Schatten machen die Szene realistischer und Stimmungsvoller

##Quellen:

1. http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html _Deferred Shading - Part 1_
2. http://ogldev.atspace.co.uk/www/tutorial36/tutorial36.html _Deferred Shading - Part 2_
3. http://ogldev.atspace.co.uk/www/tutorial37/tutorial37.html _Deferred Shading - Part 3_
4. http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/ _Normal_Mapping_
5. http://ogldev.atspace.co.uk/www/tutorial40/tutorial40.html _Shadow Volumes_
6. http://http.developer.nvidia.com/GPUGems3/gpugems3_ch11.html _Efficient and Robust Shadow Volumes Using Hierarchical Occlusion Culling and Geometry Shaders_
7. http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html _Skeletal Animation With Assimp_
8. http://http.developer.nvidia.com/GPUGems3/gpugems3_ch28.html _Practical Post-Process Depth of Field_
9. http://blog.evoserv.at/index.php/2012/12/hemispherical-screen-space-ambient-occlusion-ssao-for-deferred-renderers-using-openglglsl/
10. http://kalogirou.net/2006/05/20/how-to-do-good-bloom-for-hdr-rendering/
11. http://www.punkuser.net/vsm/vsm_paper.pdf
12. http://http.developer.nvidia.com/GPUGems3/gpugems3_ch08.html
