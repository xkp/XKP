<xs:o3d>
    <resources>
        <texture href="ui.png" required="true"/>
        <texture href="ui/shoot_up.png" required="true"/>
        <texture href="ui/shoot_down.png" required="true"/>
        <texture href="ui/shot_up.png" required="true"/>
        <texture href="ui/shot_down.png"  required="true"/>

        <model href="scene1.o3dtgz"/>
        <model_pack href="guns.xml"/>
        <model href="male1.o3dtgz"/>
        <animation href="draw.anim"/>
        <animation href="shoot.anim"/>

        <model href="sofa1.o3dtgz" at="2:00"/>
    </resources>

    <ui fullscreen="true" code="ui_test.xs">
        <switch name="toolbar" active="camera" placement="top" height="35">
            <div name="camera">
                <tool_button name="shoot" placement="right" width="35" up="ui/shoot_up.png" down="ui/shoot_down.png"/>
                <div placement="right" width="3"/>
                <tool_button name="shot" placement="right" width="35" up="ui/shot_up.png" down="ui/shot_down.png"/>
            </div>
        </switch>
    </ui>

    <scene model="scene1.o3dtgz">
        <loading_screen>
            <label placement="center" caption="Please wait..."/>
        </loading_screen>

        <character name="Joe" model="male1.o3dtgz" x="50" y="50" z="0" code="joe.xs">
            <sequence name="shoot">
                <at time="0">
                    <body_turn degrees="90" length="draw_weapon.length"/>
                    <serial>
                        <animation href="draw.anim"/>
                        <script code="pistol.visible = true;"/>
                        <animation name="shoot_anim" href="shoot.anim">
                            <target x="200" y="200" z="0"/>
                        </animation>
                    </serial>
                </at>
            </sequence>
            <sequence name="shot">
                <at time="0" length="10">
                    <transition target="Joe" channel="pose" to="animations['Shot'].frames[0]"/>
                </at>

                <at time="10">
                    <animation name="shot_anim" href="draw.anim"/>
                </at>
            </sequence>
            <right_hand>
                <attachment name="pistol" x="-20" model="Hand Pistol" visible="false"/>
            </right_hand>
        </character>
    </scene>
</xs:o3d>
