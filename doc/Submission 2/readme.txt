# Demo: Disco Fever

__DoDa Productions__

_VU Echtzeitgraphik WS2014 3.0, 186.140_

Dominik Schörkhuber (1027470) & David Pfahler (1126287)

Studienkennzahl: 033 532

## Kurzbeschreibung

Eine gruslige Musik ertönt und die Kamera fährt durch eine düster beleuchtete altmodische Halle. Plötzlich ändert sich die Musik und die Kamera schwenkt ins zentrum der Halle wo ein Hellknight (Doom) beginnt zu dem Lied "Gangnamstyle" zu tanzen, dabei wird die Szene in sich wild bewegenden Discoleuchten erläuchtet.

## Effekte

Im folgenden werden die Effekte wie in Abgabe 0 aufgeführt. Bei jedem Effekt befindet sich ein Punkt Status, der den aktuellen Status des Effektes in unserer Demo zeigt.

### Deferred Rendering [1,2,3]

Durch das Deferred Shading wollen wir die Szene mit mehreren dynamischen Lichtern beleuchtet werden. 
Zur Beleuchtung verwenden wir das Blinn Phong Beleuchtungsmodell

__Status__: Fertig. Erkennbar sollte dieser Effekt nicht sein. Erkennbar wird es werden wenn in der Finalen Abgabe noch einige weitere Lichtquellen hinzugefügt werden, welche dann weniger Performance benötigen und die Demo bei einer akzeptablen Framerate bleibt.

### Bump Mapping [4]

Auf allen Oberflächen 

__Status__: Fertig. Erkennbar an den meisten Oberflächen.

### Variance Shadow Maps - Softshadows [11,12]

Auf allen Lichtquellen

__Status__: Fertig. Erkennbar an allen Oberflächen

### Skeleton Animation [7]

Für die Tanzbewegungen der Monster

__Status__: Fertig. Erkennbar an der Bewegung des Monsters

### Depth of Field - Postprocessing [8]

Abhängig vom Kamera Focus werden Bildteile blurry dargestellt

__Status__: Doch nicht implementiert.

### SSAO - Postprocessing [9]

Screenspace Ambient Occulsion um Verdeckungen naher Objekte zu simulieren 

__Status__: Fertig. An Kanten von Modellen zu betrachten. Durch F6 ein und ausblendbar

### Bloom - Postprocessing [10]

Soll einen HDR Effekt simulieren und Lichtquellen durch Bloom darstellen

__Status__: Fertig. Für diese Abgabe sogar etwas zu stark dargestellt, damit es gut zu betrachten ist. Durch F4 ein und ausblendbar

## Erweiterung

Falls noch Zeit ist

### Shadow Volumes - Hardshadows [5,6]

Schatten machen die Szene realistischer und Stimmungsvoller

__Status__: Nicht implementiert.

## Kamera

Die Kamera bewegt sich auf einer kubisch hermiteschen Spline durch die Szene. 

Alternativ kann der User mit der Taste __F3__ in einen frei beweglichen Kameramodus wechseln bei der die Kamera mit den Tasten __W,A,S,D__ bewegt werden kann und die Blickrichtung mit der Maus geändert werden kann. Außerdem kann mit __F6__ der Post-Processing Effekt SSAO und mit __F4__ Bloom ein und ausgeschaltet werden.

## Abgaberechner

Dieser Build der Demo wurde am 19.01. auf dem Abgaberechner "__Hal__" mit der __Nvidia__ Grafikkarte getestet.

## Ausführbare Datei

Die Ausführbare Datei unserer Demo befindet sich im Unterordner bin/Candy.exe

## Video

Das File "video.mp4" zeigt ein Video der Demo

## Screenshots

![Screenshot 1](screen1.png "Screenshot 1")


![Screenshot 2](screen2.png "Screenshot 2")


![Screenshot 3](screen3.png "Screenshot 3")



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
