import QtQuick
import QtQuick3D
import QtQuick3D.Physics
import QtQuick3D.Helpers
import QtQuick.Controls
import QtQuick.Layouts
import QtQml
import DicePhysics

Item {
    id: root

    required property QtObject ctrl

    readonly property int selectedCount: ma.selection.length
    property bool denyClick: false
    property real factor: 1.0
    required property real parentWidth
    required property real parentHeight

    property real halfWidth: parentWidth/2
    property real halfHeight: parentHeight/2
    property real ceilling: 500.
    property real side: 100.
    property alias rectSelect: ma.rectSelect

    function selectAll() {
        ma.clear()
        for(let i = 0; i < dicePool.count; ++i)
        {
            let obj = dicePool.objectAt(i)
            ma.select(obj)
        }
    }

    function resetSelection() {
        ma.clear()
    }



    PhysicsWorld {
        id: physicsWorld
        running: true
        enableCCD: true
        scene: viewport.scene
        gravity: Qt.vector3d(0, -980.7, 0)
        typicalLength: 1
        typicalSpeed: 500
        minimumTimestep: 15
        maximumTimestep: 20
    }

    View3D {
        id: viewport
        anchors.fill: parent
        camera: camera

        environment: SceneEnvironment {
            clearColor: "transparent"
            backgroundMode: SceneEnvironment.Transparent
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
            lightProbe: proceduralSky
        }

        Texture {
            id: proceduralSky
            textureData: ProceduralSkyTextureData {
                sunLongitude: -115
                groundBottomColor : Qt.rgba(0.5, 0.5, 0.5, 0.5)
            }
        }

        Node {
            id: scene

            OrthographicCamera {
                id: camera
                clipFar: 300
                clipNear: 1
                position: Qt.vector3d(0.0, 300.0, 0)
                eulerRotation.x: -90 //: Qt.vector3d(-87, -0, 0)
            }

            DirectionalLight {
                eulerRotation: Qt.vector3d(0, -1, 0)//Qt.vector3d(-45, 25, 0)
                castsShadow: true
                brightness: 1
                shadowMapQuality: Light.ShadowMapQualityVeryHigh
            }


            StaticRigidBody {
                id: table

                position: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(root.parentWidth/100, 1., root.parentHeight/100)
                sendContactReports: true
                Model {
                    source: "#Cube"
                    materials: DefaultMaterial {
                        opacity: 0.0
                        diffuseColor: "#aa5555"
                    }
                }
                collisionShapes: BoxShape {
                    extents: Qt.vector3d(root.parentWidth, root.side, root.parentHeight)
                    enableDebugDraw: true
                }
            }

            StaticRigidBody {
                id: northWall

                position: Qt.vector3d(0, root.ceilling/2, -root.halfHeight-(root.side/2))
                scale: Qt.vector3d(root.parentWidth/100, root.ceilling/100, 1.)
                sendContactReports: true
                Model {
                    source: "#Cube"
                    materials: DefaultMaterial {
                        opacity: 0.2
                        diffuseColor: "#2222FF"
                    }
                }
                collisionShapes: BoxShape {
                    extents: Qt.vector3d(root.parentWidth, root.ceilling, root.side)
                    enableDebugDraw: true
                }
            }

            StaticRigidBody {
                id: southWall

                position: Qt.vector3d(0, root.ceilling/2, root.halfHeight+(root.side/2))
                scale: Qt.vector3d(root.parentWidth/100, root.ceilling/100, 1.)
                sendContactReports: true
                Model {
                    source: "#Cube"
                    materials: DefaultMaterial {
                        opacity: 0.2
                        diffuseColor: "#880000"
                    }
                }
                collisionShapes: BoxShape {
                    extents: Qt.vector3d(root.parentWidth, root.ceilling, root.side)
                    enableDebugDraw: true
                }
            }

            StaticRigidBody {
                id: westWall

                position: Qt.vector3d(-root.halfWidth-(root.side/2), root.ceilling/2, 0)
                scale: Qt.vector3d(1., root.ceilling/100, root.parentHeight/100)
                sendContactReports: true
                Model {
                    id: wwModel
                    source: "#Cube"
                    materials: DefaultMaterial {
                        opacity: 0.2
                        diffuseColor: "#00FF00"
                    }
                }
                collisionShapes: BoxShape {
                    extents: Qt.vector3d(root.side, root.ceilling, root.parentHeight)
                    enableDebugDraw: true
                }
            }

            StaticRigidBody {
                id: aestWall

                position: Qt.vector3d(root.halfWidth+(root.side/2), root.ceilling/2, 0)
                scale: Qt.vector3d(1., root.ceilling/100, root.parentHeight/100)
                sendContactReports: true
                Model {
                    id: model
                    source: "#Cube"
                    materials: DefaultMaterial {
                        opacity: 0.2
                        diffuseColor: "#888800"
                    }
                }
                collisionShapes: BoxShape {
                    extents: Qt.vector3d(root.side, root.ceilling, root.parentHeight)
                    enableDebugDraw: true
                }
            }

            StaticRigidBody {
                id: ceilling

                position: Qt.vector3d(0, root.ceilling, 0)
                scale: Qt.vector3d(root.parentWidth/100, 1., root.parentHeight/100)

                sendContactReports: true
                Model {
                    source: "#Cube"
                    materials: DefaultMaterial {
                        opacity: 0.2
                        diffuseColor: "#88EE90"
                    }
                }

                collisionShapes: BoxShape {
                    //source: model.source
                    extents: Qt.vector3d(root.parentWidth, root.side, root.parentHeight)
                    enableDebugDraw: true
                }
            }

            /*AxisHelper {
                //opacity: 0.3
                visible: false
            }*/


            Component {
                id: genericDiceComp
                RegularDice {
                    scaleFactor: root.factor
                    dice3DCtrl: root.ctrl
                    parentWidth: root.parentWidth
                    parentHeight: root.parentHeight
                }
            }

            Repeater3D {
                id: dicePool
                model: root.ctrl.model
                delegate: genericDiceComp
                function restore() {
                    for (let i = 0; i < count; i++) {
                        objectAt(i).restore()
                    }
                }
            }
        } // scene
        Rectangle {
            id: rectSelector
            visible: ma.rectSelect
            border.width: 2
            border.color: "red"
            color: "transparent"
            onVisibleChanged: {
                x = 0
                y = 0
                width = 0
                height = 0
            }
        }
        MouseArea {
            id: ma
            anchors.fill: parent
            enabled: !root.denyClick //&& !root.ctrl.expectRoll
            Connections {
                target: root.ctrl
                function onCountChanged() {
                    if(root.ctrl.count === 0)
                        ma.clear();
                }
            }
            propagateComposedEvents: true

            property list<DynamicRigidBody> selection
            property real xvelocity: 0.0
            property real zvelocity: 0.0
            property real xpos: 0.0
            property real ypos: 0.0
            property real t: 0.0
            property bool rolling: false
            property bool rectSelect: false

            property vector3d formerPosition

            function clear() {
                while(ma.selection.length > 0) {
                    let target = ma.selection.pop();
                    if(target)
                        target.selected = false
                }
            }

            function prepareSelection(point) {
                ma.rolling = true
                ma.selection.forEach(body => {
                        if(!body)
                            return;
                        body.isKinematic = true
                        body.kinematicRotation = body.rotation
                        body.kinematicPosition =  Qt.vector3d(body.position.x, 50, body.position.z)
                });
                formerPosition = point
            }
            function releaseSelection(point, velocity) {
                ma.selection.forEach(body => {
                         if(!body)
                             return;
                        body.kinematicPosition =  Qt.vector3d(point.x, 50, point.z)
                        body.isKinematic = false
                        body.applyCentralImpulse(velocity)
                        body.applyImpulse(velocity, Qt.vector3d(0, 10, 0))
                });
                ma.rolling = false
            }

            function moveSelection(point) {
                const distX= point.x - formerPosition.x
                const distZ = point.z - formerPosition.z
                ma.selection.forEach(body => {
                        if(!body)
                            return;
                        body.kinematicPosition =  Qt.vector3d(body.kinematicPosition.x + distX, body.kinematicPosition.y, body.kinematicPosition.z+ distZ)
                });
                formerPosition = point
            }

            function select(target) {
                if(!ma.selection.includes(target))
                    ma.selection.push(target)
                target.selected = true
            }

            function unselect(target) {
                const idx = ma.selection.indexOf(target)
                if(idx < 0)
                    return;

                ma.selection.splice(idx, 1)
                target.selected = false
            }

            function computeSelectionFromRect() {
                ma.clear()
                const p1 = viewport.mapTo3DScene(Qt.vector3d(rectSelector.x,rectSelector.y,0))
                const p2 = viewport.mapTo3DScene(Qt.vector3d(rectSelector.x + rectSelector.width,rectSelector.y + rectSelector.height,0))
                for(let i = 0; i < dicePool.count; ++i)
                {
                    let obj = dicePool.objectAt(i)
                    //console.log("p1",p1," p2",p2," obj",obj.position," z:",obj.scenePosition)
                    if((obj.x >= p1.x) && (obj.z >= p1.z) && (obj.x <= p2.x) && (obj.z <= p2.z))
                        ma.select(obj)
                }
            }


            function initVelocity(point) {
                xpos = point.x
                ypos = point.y
                t = Date.now()
            }
            function computeVelocity(point) {
                var nx = point.x
                var ny = point.y
                var nt = Date.now()
                var distx = nx - xpos
                var disty = ny - ypos
                var interval = nt - t
                if(interval == 0)
                    return

                ma.xvelocity = distx
                ma.zvelocity = disty
                xpos = nx
                ypos = ny
                t = nt
            }

            onPressed: (mouse)=> {

                           if(ma.rectSelect) {
                               rectSelector.x = mouse.x
                               rectSelector.y = mouse.y
                               return;
                           }

                           var point = viewport.mapTo3DScene(Qt.vector3d(mouse.x, mouse.y, 0))
                           var result = viewport.pick(mouse.x, mouse.y)

                           //console.log("inside onPressed :",result.objectHit)
                           if(!result.objectHit)
                           {
                                mouse.accepted = false
                               return;
                           }
                           if(mouse.modifiers === Qt.NoModifier)
                           {
                               if(result.objectHit) {
                                   var target = result.objectHit.parent
                                   if(!target.selected) {
                                        ma.clear()
                                        select(target)
                                   }
                                   prepareSelection(point)
                                   initVelocity(mouse)
                               }
                           }
                           else if(mouse.modifiers & Qt.ControlModifier)
                           {
                               if(result.objectHit) {
                                   let target = result.objectHit.parent
                                    if(target.selected)
                                        ma.unselect(target)
                                   else
                                        ma.select(target)
                               }
                           }
                       }
            onPositionChanged: (mouse)=>{
                    if(ma.rectSelect) {
                        rectSelector.width = mouse.x-rectSelector.x
                        rectSelector.height = mouse.y-rectSelector.y
                        return;
                    }
                    if(false === ma.rolling || ma.selection.length === 0) {
                        mouse.accepted = false
                        return
                    }

                    var point = viewport.mapTo3DScene(Qt.vector3d(mouse.x, mouse.y, 0))
                    moveSelection(point)
                    computeVelocity(mouse)
                    root.ctrl.expectRoll = true
            }

            /*onClicked: (mouse) => {
                           console.log("clicked on")
                           mouse.accepted = false
                       }*/

            onReleased: (mouse)=>{
                if(ma.rectSelect) {
                    rectSelector.width = mouse.x-rectSelector.x
                    rectSelector.height = mouse.y-rectSelector.y
                    ma.computeSelectionFromRect()
                    ma.rectSelect = false
                    return;
                }
                if(false === ma.rolling || ma.selection.length === 0) {
                    mouse.accepted = false
                    return
                }
                var point = viewport.mapTo3DScene(Qt.vector3d(mouse.x, mouse.y, 0))
                const vec = Qt.vector3d(ma.xvelocity, -0.4, ma.zvelocity).normalized();
                ma.releaseSelection(point, vec)
            }
        }
    }
}
