import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import component

sdmmc_ns = cg.esphome_ns.namespace("sdmmc_s3")
SDMMCS3 = sdmmc_ns.class_("SDMMCS3", component.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SDMMCS3),
})

async def to_code(config):
    var = cg.new_Pvariable(config["id"])
    await component.register_component(var, config)
