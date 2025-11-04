import QtQuick
import QtQuick3D
import QtQuick3D.Physics
import QtMultimedia

DynamicRigidBody {
    id: root

    required property real scaleFactor
    required property color baseCol
    required property int index
    required property int type
    required property QtObject ctrl
    required property QtObject dice3DCtrl
    required property real parentWidth
    required property real parentHeight

    property bool selected: false
    onSelectedChanged: {
        ctrl.selected = root.selected
    }

    opacity: root.ctrl?.value > 0 ? 1.0 : 0.4
    massMode: DynamicRigidBody.CustomDensity
    receiveContactReports: true

    property real internalScale: selected ? 1.2 : 1.
    readonly property string diceCode: root.dice3DCtrl.diceTypeToCode(root.type)

    QtObject {
        id: internal
        property vector3d rotation: Qt.vector3d(0, 0, 0)
    }

    function moved() {
        if(!root.selected)
            return
        root.ctrl.elapsed()
    }

    function computeRotOffset(oldpos, newpos) {
        const offset = (Math.abs(newpos.x - oldpos.x) + Math.abs(newpos.y - oldpos.y) + Math.abs(newpos.z - oldpos.z))/3;
        ctrl.addRotationOffset(offset)
        return offset;
    }

    Connections {
        target: root.ctrl
        function onStableChanged() {
            if(root.ctrl.stable)
            {
                root.ctrl.rotation = root.eulerRotation
                root.ctrl.position = root.position
            }
        }
    }

    Component.onCompleted: {
        root.position = root.ctrl.position
        root.eulerRotation = root.ctrl.rotation
    }


    scale: Qt.vector3d(scaleFactor*internalScale, scaleFactor*internalScale, scaleFactor*internalScale)
    onEulerRotationChanged: {
        if(root.ctrl.stable)
            root.ctrl.rotation = root.eulerRotation
        else if(internal.rotation.fuzzyEquals(root.eulerRotation, 0.01)) // previous value: 0.005
            return;
        else
        {
            if(computeRotOffset(internal.rotation, root.eulerRotation) > 0.01) {
                moved()
            }
        }
        internal.rotation = root.eulerRotation
    }

    onPositionChanged: {
        let newPos = root.position;
        const offset = 10//root.parentWidth * 0.01
        if(root.position.y+offset < 0)
            newPos.y = 20
        if(Math.abs(root.position.x) > Math.abs(root.dice3DCtrl.width/2-offset))
            newPos.x = root.position.x > 0 ? root.dice3DCtrl.width/2-offset : -root.dice3DCtrl.width/2+offset
        if(Math.abs(root.position.z) > Math.abs(root.dice3DCtrl.height/2-offset))
            newPos.z = root.position.z > 0 ? root.dice3DCtrl.height/2-offset : -root.dice3DCtrl.height/2+offset

        if( newPos !== root.ctrl.position)
        {
            moved();
            root.ctrl.position = newPos
        }

    }

    onBodyContact: (body, positions, impulses, normals) => {
        moved()
        let volume = 0
        impulses.forEach(vector => {
                             volume += vector.length()
                         })
        diceSound.volume = volume / 2000
        if (!diceSound.playing && !diceSound.muted)
            diceSound.play()
    }

    collisionShapes: ConvexMeshShape {
        id: diceShape
        source: "qrc:/dice3d/meshes/%1.mesh".arg(root.diceCode)
    }

    Texture {
        id: normals
        source: "qrc:/dice3d/maps/%1_Normal_OpenGL.png".arg(root.diceCode)
    }

    Texture {
        id: numberFill
        source: "qrc:/dice3d/maps/%1_Base_color.png".arg(root.diceCode)
        generateMipmaps: true
        mipFilter: Texture.Linear
    }

    physicsMaterial: PhysicsMaterial {
        id: physicsMaterial
        staticFriction: 0.0001
        dynamicFriction: 0.5
        restitution: 0.000001
    }

    Model {
        id: thisModel
        pickable: true
        source: diceShape.source
        materials: PrincipledMaterial {
            metalness: 1.0
            roughness: 0.4
            baseColor: baseCol
            emissiveMap: numberFill
            emissiveFactor: Qt.vector3d(1, 1, 1)
            normalMap: normals
            normalStrength: 0.75
        }
    }

    SoundEffect {
        id: diceSound
        loops: 0
        muted: true//root.dice3DCtrl.muted || !root.dice3DCtrl.displayed
        source: "qrc:/dice3d/sounds/onedice.wav"
    }
}
