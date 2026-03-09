import esphome.codegen as cg
import esphome.config_validation as cv

sdmmc_ns = cg.esphome_ns.namespace("sdmmc")
SDMMC = sdmmc_ns.class_("SDMMC", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SDMMC),
})

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID()])
    await cg.register_component(var, config)
