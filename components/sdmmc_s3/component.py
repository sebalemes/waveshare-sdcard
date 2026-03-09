import esphome.codegen as cg
import esphome.config_validation as cv

sdmmc_s3_ns = cg.esphome_ns.namespace("sdmmc_s3")
SDMMCS3 = sdmmc_s3_ns.class_("SDMMCS3", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SDMMCS3),
})

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID()])
    await cg.register_component(var, config)
