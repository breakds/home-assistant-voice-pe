import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import microphone
from esphome.const import CONF_ID, CONF_MICROPHONE

DEPENDENCIES = ["microphone", "network"]

udp_audio_stream_ns = cg.esphome_ns.namespace("udp_audio_stream")
UDPAudioStream = udp_audio_stream_ns.class_("UDPAudioStream", cg.Component)

CONF_TARGET_IP = "target_ip"
CONF_TARGET_PORT = "target_port"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UDPAudioStream),
    cv.Required(CONF_MICROPHONE): cv.use_id(microphone.Microphone),
    cv.Required(CONF_TARGET_IP): cv.string,
    cv.Optional(CONF_TARGET_PORT, default=5555): cv.port,
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    mic = await cg.get_variable(config[CONF_MICROPHONE])
    cg.add(var.set_microphone(mic))
    cg.add(var.set_target_ip(config[CONF_TARGET_IP]))
    cg.add(var.set_target_port(config[CONF_TARGET_PORT]))
