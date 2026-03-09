import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import sensor, text_sensor, binary_sensor

waveshare_sdmmc_ns = cg.esphome_ns.namespace("waveshare_sdmmc")
WaveshareSDMMC = waveshare_sdmmc_ns.class_("WaveshareSDMMC", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WaveshareSDMMC),

    cv.Optional("total_space"): sensor.sensor_schema(),
    cv.Optional("free_space"): sensor.sensor_schema(),
    cv.Optional("mounted"): binary_sensor.binary_sensor_schema(),
    cv.Optional("last_error"): text_sensor.text_sensor_schema(),
    cv.Optional("last_read"): text_sensor.text_sensor_schema(),

}).extend(cv.COMPONENT_SCHEMA)

# Ação para atualizar sensores manualmente
UPDATE_ACTION_SCHEMA = cv.Schema({})

@automation.register_action("waveshare_sdmmc.update_sensors", UPDATE_ACTION_SCHEMA)
async def update_sensors_action(config, action_id, template_arg):
    var = cg.new_Pvariable(action_id, template_arg)
    return var

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID()])
    await cg.register_component(var, config)

    if "total_space" in config:
        sens = await sensor.new_sensor(config["total_space"])
        cg.add(var.set_total_space_sensor(sens))

    if "free_space" in config:
        sens = await sensor.new_sensor(config["free_space"])
        cg.add(var.set_free_space_sensor(sens))

    if "mounted" in config:
        sens = await binary_sensor.new_binary_sensor(config["mounted"])
        cg.add(var.set_mounted_sensor(sens))

    if "last_error" in config:
        sens = await text_sensor.new_text_sensor(config["last_error"])
        cg.add(var.set_last_error_sensor(sens))

    if "last_read" in config:
        sens = await text_sensor.new_text_sensor(config["last_read"])
        cg.add(var.set_last_read_sensor(sens))
