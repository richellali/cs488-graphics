rootNode = gr.node('root')
rootNode:translate(0.0, 1.0, -10.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
rootNode:add_child(torso)
torso:set_material(blue)
torso:scale(0.9, 1.5, 0.9)

-- shoulder
shoulderJoint = gr.node('shoulderJoint')
rootNode:add_child(shoulderJoint)
shoulderJoint:translate(0.0, 1.1, 0.0)

shoulder = gr.mesh('sphere', 'shoulder')
shoulderJoint:add_child(shoulder)
shoulder:scale(1.4, 0.5, 0.5)
shoulder:set_material(blue)

-- neckShoulder - neck - neckHead - head
-- head & neck
neckShoulderJoint = gr.joint('neckShoulderJoint', {-30, 0, 30}, {0, 0, 0})
shoulderJoint:add_child(neckShoulderJoint)
neckShoulderJoint:translate(0, 0.3, 0)

neck = gr.mesh('sphere', 'neck')
neckShoulderJoint:add_child(neck)
neck:scale(0.3, 0.5, 0.3)
neck:translate(0, 0.5, 0)
neck:set_material(white)

neckHeadJoint = gr.joint('neckHeadJoint', {0, 0, 0}, {-30, 0, 30})
neckShoulderJoint:add_child(neckHeadJoint)
neckHeadJoint:translate(0, 0.7, 0)

head = gr.mesh('sphere', 'head')
neckHeadJoint:add_child(head)
head:scale(0.9, 0.9, 0.9)
head:translate(0.0, 0.5, 0)
head:set_material(white)

nose = gr.mesh('sphere', 'nose')
head:add_child(nose)
nose:scale(0.25, 0.25, 0.25)
nose:translate(0.0, 0.0, 0.85)
nose:set_material(blue)

-- left arm 
leftUpperArmJoint = gr.joint('leftShoulderArm', {-70, 0, 70}, {0, 0, 0})
shoulderJoint:add_child(leftUpperArmJoint)
leftUpperArmJoint:translate(-1.4, 0.1, 0)


leftUpperArm = gr.mesh('sphere', 'leftUpperArm')
leftUpperArmJoint:add_child(leftUpperArm)
leftUpperArm:scale(0.3, 1.0, 0.3)
leftUpperArm:translate(0.0, -1.0, 0.0)
leftUpperArm:set_material(white)

leftUpperForearmJoint = gr.joint('leftUpperForearm', {-70, 0, 0}, {0, 0, 0})
leftUpperArmJoint:add_child(leftUpperForearmJoint)
leftUpperForearmJoint:translate(0.0, -1.9, 0.0)

leftForearm = gr.mesh('sphere', 'leftForearm')
leftUpperForearmJoint:add_child(leftForearm)
leftForearm:scale(0.25, 0.85, 0.25)
leftForearm:translate(0.0, -0.85, 0.0)
leftForearm:set_material(white)

leftArmHandJoint = gr.joint('leftArmHand', {-30, 0, 30}, {0, 0, 0})
leftUpperForearmJoint:add_child(leftArmHandJoint)
leftArmHandJoint:translate(0.0, -1.6, 0.0)

leftHand = gr.mesh('sphere', 'leftHand')
leftArmHandJoint:add_child(leftHand)
leftHand:scale(0.3, 0.3, 0.3)
leftHand:translate(0.0, -0.3, 0.0)
leftHand:set_material(blue)


-- right arm 
rightUpperArmJoint = gr.joint('rightShoulderArm', {-70, 0, 70}, {0, 0, 0})
shoulderJoint:add_child(rightUpperArmJoint)
rightUpperArmJoint:translate(1.4, 0.1, 0)


rightUpperArm = gr.mesh('sphere', 'rightUpperArm')
rightUpperArmJoint:add_child(rightUpperArm)
rightUpperArm:scale(0.3, 1.0, 0.3)
rightUpperArm:translate(0.0, -1.0, 0.0)
rightUpperArm:set_material(white)

rightUpperForearmJoint = gr.joint('rightUpperForearm', {-70, 0, 0}, {0, 0, 0})
rightUpperArmJoint:add_child(rightUpperForearmJoint)
rightUpperForearmJoint:translate(0.0, -1.9, 0.0)

rightForearm = gr.mesh('sphere', 'rightForearm')
rightUpperForearmJoint:add_child(rightForearm)
rightForearm:scale(0.25, 0.85, 0.25)
rightForearm:translate(0.0, -0.85, 0.0)
rightForearm:set_material(white)

rightArmHandJoint = gr.joint('rightArmHand', {-30, 0, 30}, {0, 0, 0})
rightUpperForearmJoint:add_child(rightArmHandJoint)
rightArmHandJoint:translate(0.0, -1.6, 0.0)

rightHand = gr.mesh('sphere', 'rightHand')
rightArmHandJoint:add_child(rightHand)
rightHand:scale(0.3, 0.3, 0.3)
rightHand:translate(0.0, -0.3, 0.0)
rightHand:set_material(blue)

-- hip
hipJoint = gr.node('hipJoint')
rootNode:add_child(hipJoint)
hipJoint:translate(0.0, -1.4, 0.0)

hip = gr.mesh('sphere', 'hip')
hipJoint:add_child(hip)
hip:scale(1.1, 0.4, 0.4)
hip:set_material(blue)

-- left leg
leftHipThighJoint = gr.joint('leftHipThighJoint', {-70, 0, 70}, {0, 0, 0})
hipJoint:add_child(leftHipThighJoint)
leftHipThighJoint:translate(-0.7, -0.2, 0)


leftThigh = gr.mesh('sphere', 'leftThigh')
leftHipThighJoint:add_child(leftThigh)
leftThigh:scale(0.4, 1.2, 0.4)
leftThigh:translate(0.0, -1.2, 0.0)
leftThigh:set_material(white)

leftThighCalfJoint = gr.joint('leftThighCalfJoint', {0, 0, 70}, {0, 0, 0})
leftHipThighJoint:add_child(leftThighCalfJoint)
leftThighCalfJoint:translate(0.0, -2.3, 0.0)

leftCalf = gr.mesh('sphere', 'leftCalf')
leftThighCalfJoint:add_child(leftCalf)
leftCalf:scale(0.3, 1.0, 0.3)
leftCalf:translate(0.0, -1.0, 0.0)
leftCalf:set_material(white)

leftCalfFootJoint = gr.joint('leftCalfFootJoint', {-45, 0, 45}, {0, 0, 0})
leftThighCalfJoint:add_child(leftCalfFootJoint)
leftCalfFootJoint:translate(0.0, -2.0, 0.0)

leftFoot = gr.mesh('sphere', 'leftFoot')
leftCalfFootJoint:add_child(leftFoot)
leftFoot:scale(0.3, 0.3, 0.6)
leftFoot:translate(0.0, -0.2, 0.3)
leftFoot:set_material(blue)

-- right leg
rightHipThighJoint = gr.joint('rightHipThighJoint', {-70, 0, 70}, {0, 0, 0})
hipJoint:add_child(rightHipThighJoint)
rightHipThighJoint:translate(0.7, -0.2, 0)


rightThigh = gr.mesh('sphere', 'rightThigh')
rightHipThighJoint:add_child(rightThigh)
rightThigh:scale(0.4, 1.2, 0.4)
rightThigh:translate(0.0, -1.2, 0.0)
rightThigh:set_material(white)

rightThighCalfJoint = gr.joint('rightThighCalfJoint', {0, 0, 70}, {0, 0, 0})
rightHipThighJoint:add_child(rightThighCalfJoint)
rightThighCalfJoint:translate(0.0, -2.3, 0.0)

rightCalf = gr.mesh('sphere', 'rightCalf')
rightThighCalfJoint:add_child(rightCalf)
rightCalf:scale(0.3, 1.0, 0.3)
rightCalf:translate(0.0, -1.0, 0.0)
rightCalf:set_material(white)

rightCalfFootJoint = gr.joint('rightCalfFootJoint', {-45, 0, 45}, {0, 0, 0})
rightThighCalfJoint:add_child(rightCalfFootJoint)
rightCalfFootJoint:translate(0.0, -2.0, 0.0)

rightFoot = gr.mesh('sphere', 'rightFoot')
rightCalfFootJoint:add_child(rightFoot)
rightFoot:scale(0.3, 0.3, 0.6)
rightFoot:translate(0.0, -0.2, 0.3)
rightFoot:set_material(blue)

return rootNode