import json
""" Load up the schema and the current info for the set keyboard then display
    Via a TUI.
    TODO:
    add in dictionary indexing to access the schemas"""


def test_for_type(test_properties, value):
    return type(value) == test_properties


class Schema_Base_Object:
    TYPE_NAME = "Default"

    def __init__(self, node, name, is_additional_property=False):
        self.name = name
        self.limits = {}
        self.value = None
        self.implimented = False
        self.is_additional_property = is_additional_property
        for key in node:
            if key is not "type":
                self.limits[key] = node[key]

    def __str__(self):
        return f"{self.name} ({type(self).TYPE_NAME}) : \
{self.value}\n"

    def pretty_print(self, depth=0, print_all=True):
        if print_all or self.implimented:
            return f"{depth * '   '}{self}"
        return ""

    def is_valid_value(self, value):
        return True

    def update_value(self, value):
        is_valid = self.is_valid_value(value)
        if is_valid:
            self._set_value_(value)
        else:
            print(f"Value {value} is not valid for {self.name} of type\
{type(self).TYPE_NAME}")
        return is_valid

    def _set_value_(self, value):
        self.value = value
        self.implimented = value is not None


class Schema_String(Schema_Base_Object):
    TYPE_NAME = "String"


class Schema_Boolean(Schema_Base_Object):
    TYPE_NAME = "Boolean"


class Schema_Number(Schema_Base_Object):
    TYPE_NAME = "Number"


class Schema_Array(Schema_Base_Object):
    TYPE_NAME = "Array"


class Schema_One_Of(Schema_Base_Object):
    TYPE_NAME = "One Of"
    """ Needs to check if the input matches any of the templates """


class Schema_Object(Schema_Base_Object):
    TYPE_NAME = "Object"
    SCHEMA_PRINT_INDENTATION = 4
    @staticmethod
    def node_factory(node, name, is_additional_property=False):
        if "oneOf" in node:
            return Schema_One_Of(node, name, is_additional_property)
        elif node["type"] == "string":
            return Schema_String(node, name, is_additional_property)
        elif node["type"] == "object":
            return Schema_Object(node, name, is_additional_property)
        elif node["type"] == "boolean":
            return Schema_Boolean(node, name, is_additional_property)
        elif node["type"] == "number":
            return Schema_Number(node, name, is_additional_property)
        elif node["type"] == "array":
            return Schema_Array(node, name, is_additional_property)
        return Schema_Base_Object(node, name, is_additional_property)

    def __init__(self, schema_data, name, is_additional_property=False):
        self.properties = {}
        self.additional_properties = None
        self.additional_properties_array = []
        self.name = name
        self.implimented = False
        self.is_additional_property = is_additional_property
        self.load_properties(schema_data)
        self.load_additional_properties(schema_data)

    def load_properties(self, schema_data):
        if "properties" in schema_data:
            for property in schema_data["properties"]:
                self.properties[property] = (
                        self.node_factory(
                            schema_data["properties"][property],
                            property
                        )
                    )

    def load_additional_properties(self, schema_data):
        """ Don't make anything if it's not specified or it's false
            Otherwise the additionalProperties will contain a single Entry """
        if ("additionalProperties" in schema_data
                and schema_data["additionalProperties"]):
            self.additional_properties = self.node_factory(
                schema_data["additionalProperties"],
                property,
                True
            )

    def properties_to_string(self, depth, print_all=True):
        return "".join([self.properties[i].pretty_print(depth, print_all)
                        for i in self.properties])

    def additional_properties_to_string(self, depth, print_all=True):
        return (self.additional_properties.pretty_print(depth, print_all)
                if self.additional_properties else "")

    def __str__(self):
        own_string = f"{self.name} (Object)\n"
        if self.additional_properties is not None:
            own_string += " -additionalProperties:\n" + self.additional_properties_to_string(1, print_all)
        if (len(self.properties) > 0):
            own_string = own_string + " -properties:\n"
            properties = self.properties_to_string(1)
            return own_string + properties
        else:
            return own_string

    def pretty_print(self, depth=0, print_all=True):
        if print_all or self.implimented:
            indentation = depth * Schema_Object.SCHEMA_PRINT_INDENTATION * " "
            own_string = f"{indentation}{self.name} (Object)\n"
            if self.additional_properties is not None:
                own_string += indentation + " -additionalProperties:\n" + self.additional_properties_to_string(depth + 1, print_all)
            if (len(self.properties) > 0):
                own_string = f"{own_string}{indentation} -properties:\n"
                properties = self.properties_to_string(depth + 1, print_all)
                return own_string + properties
            else:
                return own_string
        else:
            return ""

    def add_additional_property(self, value):

        self.additional_properties_array

    def _set_value_(self, value):
        """ Set all the values for self and children """
        for key in value:
            try:
                if not self.properties[key].update_value(value[key]):
                    print(f"was unable to update {key}")
            except KeyError:

                print(f"{key} may be an additional property")
        self.implimented = True


class Schema:
    def __init__(self, schema_template, schema_values):
        " Go through each object and create the tree "
        self.schema_url = schema_template["$schema"]
        self.schema_id = schema_template["$id"]
        self.schema_title = schema_template["title"]
        self.schema = Schema_Object(schema_template, self.schema_title)
        self.schema.update_value(schema_values)
        print(self.schema.pretty_print(0, False))


def load_json(path):
    schema_data = open(path, "r")
    return json.loads(schema_data.read())


def load_schema_template():
    SCHEMA_PATH = "../data/schemas/keyboard.jsonschema"
    return load_json(SCHEMA_PATH)


def load_schema_values(keyboard):
    return load_json(keyboard_info_to_path(keyboard))


def keyboard_info_to_path(keyboard):
    return "../keyboards/dztech/dz65rgb/info.json"


if __name__ == "__main__":
    """ Load up the schema and the current info """
    schema_template = load_schema_template()
    schema_values = load_schema_values(None)
    schema = Schema(schema_template, schema_values)
